#include "OptBase.h"

///@todo split this file into static and non-static

std::mutex
    OptBase::mutexQueueTodo,
    OptBase::mutexQueueCalculated,
    OptBase::mutexQueueFinished;

std::queue< std::pair<OptValue, OptBase*> >
    OptBase::queueTodo,
    OptBase::queueCalculated,
    OptBase::queueFinished;

OptBase::OptBase(const std::vector<OptBoundary> &optBoundaries,
                 unsigned int maxCalculations,
                 CalculatorBase* pCalculator,
                 OptTarget optTarget) :
    maxCalculations(maxCalculations),
    currentCalculation(0), ///@todo or start at 1?
    optBoundaries(optBoundaries),
    pCalculator(pCalculator),
    optTarget(optTarget),
    targetValue(0.0)
{


}

OptBase::~OptBase()
{

}

bool OptBase::optimise() ///@todo split this method into two [#1 only put new value onto to-calc queue ||| #2 save the result and move to next queue]
{
    OptValue newOptValue;

    if(currentCalculation >= maxCalculations) ///@todo might have to fix this condition
        return false;

    if(currentCalculation == 0) ///@todo care in case currentCalculation gets initalised with 1
    {
        bestCalculation.result = bad_value();
    }

    if(true) ///@todo currently this is always true, allow algorithms to stop somehow
    {
        newOptValue = get_next_value();
        ///@todo put new value in todo queue
    }
    return true;
}

void OptBase::add_finished_calculation(OptValue optValue, OptBase* pOptBase)
{
    mutexPreviousCalculations.lock();
    previousCalculations.push_back(optValue);
    mutexPreviousCalculations.unlock();
    mutexQueueFinished.lock();
    queueFinished.push({optValue, pOptBase});
    mutexQueueFinished.unlock();

    if(result_better(optValue, bestCalculation))
        bestCalculation = optValue;
}

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

bool OptBase::result_better(const OptValue &result, const OptValue &other) const
{
    switch(optTarget)
    {
        case MINIMIZE:
            return result.result < other.result;

        case MAXIMIZE:
            return result.result > other.result;

        case APPROACH:
            return abs(targetValue - result.result) < abs(targetValue - other.result);

        case DIVERGE:
            return abs(targetValue - result.result) > abs(targetValue - other.result);

        default: //MINIMIZE
            return result.result < other.result;
    }
}

void OptBase::run_optimisations(unsigned int maxThreads)
{

}

T OptBase::random_factor()
{
    return rand()/(T)(RAND_MAX);
}

void OptBase::threaded_work()
{
    while(true) ///@todo there has to be a check whether there still are calculations
    {
        if(available_todo())
        {
            ///@todo possible race condition here. Make it that pop checks for availability itself (return bool and have the value be a reference which is changed bool pop(out) )
            ///@todo or add another mutex which only locks for this check mutex availabilityCheck
            std::pair <OptValue, OptBase*> todo = pop_todo();
            OptValue optValue = todo.first;
            OptBase* pOptBase = todo.second;

            pOptBase->pCalculator->calculate(optValue);



        }



        std::this_thread::sleep_for(std::chrono::milliseconds(10)); ///@todo sleep time must be defineable by library user [maybe add as paramter to run_optimisations]
    }

}

void OptBase::push_todo(OptValue optValue, OptBase* pOptBase)
{
    mutexQueueTodo.lock();
    queueTodo.push({optValue, pOptBase});
    mutexQueueTodo.unlock();
}

void OptBase::push_calculated(OptValue optValue, OptBase *pOptBase)
{
    mutexQueueCalculated.lock();
    queueCalculated.push({optValue, pOptBase});
    mutexQueueCalculated.unlock();
}

void OptBase::push_finished(OptValue optValue, OptBase *pOptBase)
{
    mutexQueueFinished.lock();
    queueFinished.push({optValue, pOptBase});
    mutexQueueFinished.unlock();
}

bool OptBase::available_todo()
{
    bool out(false);

    mutexQueueTodo.lock();
    out = !queueTodo.empty();
    mutexQueueTodo.unlock();

    return out;
}

bool OptBase::available_calculated()
{
    bool out(false);

    mutexQueueCalculated.lock();
    out = !queueCalculated.empty();
    mutexQueueCalculated.unlock();

    return out;
}

bool OptBase::available_finished()
{
    bool out(false);

    mutexQueueFinished.lock();
    out = !queueFinished.empty();
    mutexQueueFinished.unlock();

    return out;
}

std::pair<OptValue, OptBase*> OptBase::pop_todo()
{
    mutexQueueTodo.lock();
    auto out = queueTodo.front();
    queueTodo.pop();
    mutexQueueTodo.unlock();
    return out;
}

std::pair<OptValue, OptBase*> OptBase::pop_calculated()
{
    mutexQueueCalculated.lock();
    auto out = queueCalculated.front();
    queueCalculated.pop();
    mutexQueueCalculated.unlock();
    return out;
}

std::pair<OptValue, OptBase*> OptBase::pop_finished()
{
    mutexQueueFinished.lock();
    auto out = queueFinished.front();
    queueFinished.pop();
    mutexQueueFinished.unlock();
    return out;
}

