/*
    Copyright (c) 2015 Martin Buck
    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation the rights to
    use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
    and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
    The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
    DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
    OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "OptBase.h"

namespace cppOpt
{

std::mutex
    OptBase::mutexQueueTodo,
    OptBase::mutexAvailabilityCheckTodo,
    OptBase::mutexQueueCalculated,
    OptBase::mutexFinishedCalculations,
    OptBase::mutexPOptimisers,
    OptBase::mutexLogFile;

std::queue< std::pair<OptCalculation, OptBase*> >
    OptBase::queueTodo;

std::vector< std::pair<OptCalculation, OptBase*> >
    OptBase::finishedCalculations;

std::set<OptBase*>
    OptBase::pOptimisers;

bool
    OptBase::loggingEnabled(false);


std::string
    OptBase::loggingDelimiter(""),
    OptBase::loggingLineEnd("");

std::ofstream
    OptBase::logFile;

unsigned int
    OptBase::waitTimeMs(0);

// PUBLIC ----------------------------------------------------------------------

OptBase::OptBase(const OptBoundaries &optBoundaries,
                 unsigned int maxCalculations,
                 OptSolverBase* pCalculator,
                 OptTarget optTarget,
                 OPT_T targetValue) :
    maxCalculations(maxCalculations),
    optBoundaries(optBoundaries),
    pCalculator(pCalculator),
    optTarget(optTarget),
    targetValue(targetValue)
{
    mutexPOptimisers.lock();
    pOptimisers.insert(this);
    mutexPOptimisers.unlock();
    srand( time(NULL) + rand() );
}

//------------------------------------------------------------------------------

OptBase::~OptBase()
{
    mutexPOptimisers.lock();
    pOptimisers.erase( pOptimisers.find(this) );
    mutexPOptimisers.unlock();
}

//------------------------------------------------------------------------------

void OptBase::run_optimisations(unsigned int maxThreads)
{
    //get the first to-calculate value of every optimiser
    //and push it onto the todo queue
    mutexPOptimisers.lock();
    for(const auto &pOptimiser : pOptimisers)
    {
        if(pOptimiser->previousCalculations.size() == 0)
            push_todo(pOptimiser->get_next_calculation(), pOptimiser);
    }
    mutexPOptimisers.unlock();

    std::list <std::thread> threads;

    for(unsigned int i=0; i<maxThreads; ++i)
        threads.emplace_back(  std::thread( std::bind(&OptBase::threaded_work) )  );

    for(auto &thread :threads)
        thread.join();
}

//------------------------------------------------------------------------------

unsigned int OptBase::number_optimisers()
{
    unsigned int out(0);
    mutexPOptimisers.lock();
    out = pOptimisers.size();
    mutexPOptimisers.unlock();
    return out;
}

//------------------------------------------------------------------------------

bool OptBase::enable_logging(const std::string &pathLogFile, const OptBoundaries &optBoundaries, const std::string &delimiter, const std::string &lineEnd)
{
    logFile.open(pathLogFile);
    if(logFile.fail())
        return false;
    loggingEnabled = true;
    loggingDelimiter = delimiter;
    loggingLineEnd = lineEnd;
    logFile << optBoundaries.to_string() << "RESULT" << loggingLineEnd;
    return true;
}

//------------------------------------------------------------------------------

void OptBase::set_wait_time(unsigned int timeInMs)
{
    waitTimeMs = timeInMs;
}

//------------------------------------------------------------------------------

OptCalculation OptBase::get_best_calculation(OptTarget optTarget, OPT_T targetValue)
{
    OptCalculation out;
    mutexFinishedCalculations.lock();

    if(finishedCalculations.size() == 0)
    {
        mutexFinishedCalculations.unlock();
        return out;
    }

    out = finishedCalculations[0].first;

    for(const auto &finishedCalculation : finishedCalculations)
        if(result_better(finishedCalculation.first, out, optTarget, targetValue))
            out = finishedCalculation.first;

    mutexFinishedCalculations.unlock();
    return out;
}

//------------------------------------------------------------------------------

OptCalculation OptBase::get_best_calculation() const
{
   return bestCalculation;
}

// PROTECTED -------------------------------------------------------------------

void OptBase::add_finished_calculation(OptCalculation optCalculation)
{
    previousCalculations.push_back(optCalculation);

    mutexFinishedCalculations.lock();
    finishedCalculations.push_back({optCalculation, this});
    mutexFinishedCalculations.unlock();

    if(result_better(optCalculation, bestCalculation, optTarget, targetValue))
        bestCalculation = optCalculation;
}

//------------------------------------------------------------------------------

OPT_T OptBase::bad_value() const
{
    switch(optTarget)
    {
        case MINIMIZE:
            return std::numeric_limits<OPT_T>::max();

        case MAXIMIZE:
            return std::numeric_limits<OPT_T>::min();

        case APPROACH:
            if(targetValue > 0.0)
                return std::numeric_limits<OPT_T>::min();
            else
                return std::numeric_limits<OPT_T>::max();

        case DIVERGE:
            return targetValue;

        default: //MINIMIZE
            return std::numeric_limits<OPT_T>::max();
    }
}

//------------------------------------------------------------------------------

bool OptBase::valid(const OptCalculation &optCalculation) const
{
    for(auto boundary = optBoundaries.cbegin(); boundary != optBoundaries.cend(); ++boundary)
    {
        if(
        optCalculation.get_parameter(boundary->name) < boundary->min
        ||
        optCalculation.get_parameter(boundary->name) > boundary->max)
            return false;
    }
    return true;
}

//------------------------------------------------------------------------------

bool OptBase::result_better(const OptCalculation &result, const OptCalculation &other, OptTarget optTarget, OPT_T targetValue)
{
    switch(optTarget)
    {
        case MINIMIZE:
            return result.result < other.result;

        case MAXIMIZE:
            return result.result > other.result;

        case APPROACH:
            return fabs(targetValue - result.result) < fabs(targetValue - other.result);

        case DIVERGE:
            return fabs(targetValue - result.result) > fabs(targetValue - other.result);

        default: //MINIMIZE
            return result.result < other.result;
    }
}


//------------------------------------------------------------------------------

OPT_T OptBase::random_factor()
{
    return rand()/(OPT_T)(RAND_MAX);
}

// PRIVATE ---------------------------------------------------------------------

void OptBase::threaded_work()
{
    while(true)
    {
        mutexAvailabilityCheckTodo.lock(); //the check for availability and the pop have to be atomic
        bool availableTodo = available_todo();

        if(!availableTodo)
            mutexAvailabilityCheckTodo.unlock(); //no pop will occur, can unlock directly

        else // availableTodo
        {
            std::pair <OptCalculation, OptBase*> todo = pop_todo();
            mutexAvailabilityCheckTodo.unlock(); //pop occured, can unlock now
            OptCalculation optCalculation = todo.first;
            OptBase* pOptBase = todo.second;

            pOptBase->pCalculator->calculate(optCalculation);

            if(loggingEnabled)
                log(optCalculation);

            pOptBase->add_finished_calculation(optCalculation);

            if(pOptBase->previousCalculations.size() > pOptBase->maxCalculations) ///@todo maybe be >=
                break;

            //only add the next one if there still are more
            push_todo(pOptBase->get_next_calculation(), pOptBase);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(waitTimeMs));
    }

}

//------------------------------------------------------------------------------

void OptBase::push_todo(OptCalculation optCalculation, OptBase* pOptBase)
{
    mutexQueueTodo.lock();
    queueTodo.push({optCalculation, pOptBase});
    mutexQueueTodo.unlock();
}

//------------------------------------------------------------------------------

void OptBase::push_finished(OptCalculation optCalculation, OptBase *pOptBase)
{
    mutexFinishedCalculations.lock();
    finishedCalculations.push_back({optCalculation, pOptBase});
    mutexFinishedCalculations.unlock();
}

//------------------------------------------------------------------------------

bool OptBase::available_todo()
{
    bool out(false);

    mutexQueueTodo.lock();
    out = !queueTodo.empty();
    mutexQueueTodo.unlock();

    return out;
}

//------------------------------------------------------------------------------

std::pair<OptCalculation, OptBase*> OptBase::pop_todo()
{
    mutexQueueTodo.lock();
    auto out = queueTodo.front();
    queueTodo.pop();
    mutexQueueTodo.unlock();
    return out;
}

//------------------------------------------------------------------------------

void OptBase::log(const OptCalculation &optCalculation)
{
    mutexLogFile.lock();
    logFile << optCalculation.to_string_values(loggingDelimiter) << loggingLineEnd;
    mutexLogFile.unlock();
}

//------------------------------------------------------------------------------

} // namespace cppOpt
