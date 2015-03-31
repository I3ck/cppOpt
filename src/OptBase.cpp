#include "OptBase.h"

namespace cppOpt
{

std::mutex
    OptBase::mutexQueueTodo,
    OptBase::mutexQueueCalculated,
    OptBase::mutexFinishedCalculations,
    OptBase::mutexPOptimizers,
    OptBase::mutexLogFile;

std::queue< std::pair<OptValue, OptBase*> >
    OptBase::queueTodo;

std::vector< std::pair<OptValue, OptBase*> >
    OptBase::finishedCalculations;

std::set<OptBase*>
    OptBase::pOptimizers;

bool
    OptBase::loggingEnabled(false);

std::ofstream
    OptBase::logFile;

unsigned int
    OptBase::waitTimeMs(0);

// PUBLIC ----------------------------------------------------------------------

OptBase::OptBase(const OptBoundaries &optBoundaries,
                 unsigned int maxCalculations,
                 SolverBase* pCalculator,
                 OptTarget optTarget,
                 T targetValue) :
    maxCalculations(maxCalculations),
    optBoundaries(optBoundaries),
    pCalculator(pCalculator),
    optTarget(optTarget),
    targetValue(targetValue)
{
    mutexPOptimizers.lock();
    pOptimizers.insert(this);
    mutexPOptimizers.unlock();
    srand( time(NULL) + rand() );
}

//------------------------------------------------------------------------------

OptBase::~OptBase()
{
    mutexPOptimizers.lock();
    pOptimizers.erase( pOptimizers.find(this) );
    mutexPOptimizers.unlock();
}

//------------------------------------------------------------------------------

void OptBase::run_optimisations(unsigned int maxThreads)
{
    //get the first to-calculate value of every optimizer
    //and push it onto the todo queue
    mutexPOptimizers.lock();
    for(const auto &pOptimizer : pOptimizers)
    {
        if(pOptimizer->previousCalculations.size() == 0)
            push_todo(pOptimizer->get_next_value(), pOptimizer);
    }
    mutexPOptimizers.unlock();

    std::list <std::thread> threads;

    for(unsigned int i=0; i<maxThreads; ++i)
        threads.emplace_back(  std::thread( std::bind(&OptBase::threaded_work) )  );

    for(auto &thread :threads)
        thread.join();
}

//------------------------------------------------------------------------------

unsigned int OptBase::number_optimizers()
{
    unsigned int out(0);
    mutexPOptimizers.lock();
    out = pOptimizers.size();
    mutexPOptimizers.unlock();
    return out;
}

//------------------------------------------------------------------------------

bool OptBase::enable_logging(const std::string &pathLogFile, const OptBoundaries &optBoundaries)
{
    logFile.open(pathLogFile);
    if(logFile.fail())
        return false;
    logFile << optBoundaries.to_string() << "RESULT\n";
    loggingEnabled = true;
    return true;
}

//------------------------------------------------------------------------------

void OptBase::set_wait_time(unsigned int timeInMs)
{
    waitTimeMs = timeInMs;
}

//------------------------------------------------------------------------------

OptValue OptBase::get_best_calculation(OptTarget optTarget)
{
    OptValue out;
    mutexFinishedCalculations.lock();

    if(finishedCalculations.size() == 0)
    {
        mutexFinishedCalculations.unlock();
        return out;
    }

    out = finishedCalculations[0].first;

    for(const auto &finishedCalculation : finishedCalculations)
        if(result_better(finishedCalculation.first, out, optTarget))
            out = finishedCalculation.first;

    mutexFinishedCalculations.unlock();
    return out;
}

//------------------------------------------------------------------------------

OptValue OptBase::get_best_calculation() const
{
   return bestCalculation;
}

// PROTECTED -------------------------------------------------------------------

void OptBase::add_finished_calculation(OptValue optValue)
{
    previousCalculations.push_back(optValue);

    mutexFinishedCalculations.lock();
    finishedCalculations.push_back({optValue, this});
    mutexFinishedCalculations.unlock();

    if(result_better(optValue, bestCalculation, optTarget))
        bestCalculation = optValue;
}

//------------------------------------------------------------------------------

T OptBase::bad_value() const
{
    switch(optTarget)
    {
        case MINIMIZE:
            return std::numeric_limits<T>::max();

        case MAXIMIZE:
            return std::numeric_limits<T>::min();

        case APPROACH:
            if(targetValue > 0.0)
                return std::numeric_limits<T>::min();
            else
                return std::numeric_limits<T>::max();

        case DIVERGE:
            return targetValue;

        default: //MINIMIZE
            return std::numeric_limits<T>::max();
    }
}

//------------------------------------------------------------------------------

bool OptBase::result_better(const OptValue &result, const OptValue &other, OptTarget optTarget, T targetValue)
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

T OptBase::random_factor()
{
    return rand()/(T)(RAND_MAX);
}

// PRIVATE ---------------------------------------------------------------------

void OptBase::threaded_work()
{
    while(true)
    {
        if(available_todo())
        {
            ///@todo possible race condition here. Make it that pop checks for availability itself (return bool and have the value be a reference which is changed bool pop(out) )
            ///@todo or add another mutex which only locks for this check mutex availabilityCheck
            std::pair <OptValue, OptBase*> todo = pop_todo();
            OptValue optValue = todo.first;
            OptBase* pOptBase = todo.second;

            pOptBase->pCalculator->calculate(optValue);

            if(loggingEnabled)
                log(optValue);

            pOptBase->add_finished_calculation(optValue);

            if(pOptBase->previousCalculations.size() > pOptBase->maxCalculations) ///@todo maybe be >=
                break;

            //only add the next one if there still are more
            push_todo(pOptBase->get_next_value(), pOptBase);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(waitTimeMs));
    }

}

//------------------------------------------------------------------------------

void OptBase::push_todo(OptValue optValue, OptBase* pOptBase)
{
    mutexQueueTodo.lock();
    queueTodo.push({optValue, pOptBase});
    mutexQueueTodo.unlock();
}

//------------------------------------------------------------------------------

void OptBase::push_finished(OptValue optValue, OptBase *pOptBase)
{
    mutexFinishedCalculations.lock();
    finishedCalculations.push_back({optValue, pOptBase});
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

bool OptBase::available_finished()
{
    bool out(false);

    mutexFinishedCalculations.lock();
    out = !finishedCalculations.empty();
    mutexFinishedCalculations.unlock();

    return out;
}

//------------------------------------------------------------------------------

std::pair<OptValue, OptBase*> OptBase::pop_todo()
{
    mutexQueueTodo.lock();
    auto out = queueTodo.front();
    queueTodo.pop();
    mutexQueueTodo.unlock();
    return out;
}

//------------------------------------------------------------------------------

void OptBase::log(const OptValue &optValue)
{
    mutexLogFile.lock();
    logFile << optValue.to_string_values() << std::endl;
    mutexLogFile.unlock();
}

//------------------------------------------------------------------------------

} // namespace cppOpt
