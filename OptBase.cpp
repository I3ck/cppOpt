#include "OptBase.h"

std::mutex
    OptBase::mutexQueueTodo,
    OptBase::mutexQueueCalculated,
    OptBase::mutexQueueFinished;

std::queue<OptValue>
    OptBase::queueTodo,
    OptBase::queueCalculated,
    OptBase::queueFinished;

OptBase::OptBase(const std::vector<OptBoundary> &optBoundaries,
                 unsigned int maxCalculations,
                 OptTarget optTarget) :
    maxCalculations(maxCalculations),
    optBoundaries(optBoundaries),
    optTarget(optTarget),
    targetValue(0.0)
{


}

OptBase::~OptBase()
{

}

bool OptBase::optimise()
{

}

void OptBase::add_finished_calculation(const OptValue &optValue)
{
    mutexPreviousCalculations.lock();
    previousCalculations.push_back(optValue);
    mutexPreviousCalculations.unlock();
    mutexQueueFinished.lock();
    queueFinished.push(optValue);
    mutexQueueFinished.unlock();
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

void OptBase::push_todo(const OptValue &optValue)
{
    mutexQueueTodo.lock();
    queueTodo.push(optValue);
    mutexQueueTodo.unlock();
}

void OptBase::push_calculated(const OptValue &optValue)
{
    mutexQueueCalculated.lock();
    queueCalculated.push(optValue);
    mutexQueueCalculated.unlock();
}

void OptBase::push_finished(const OptValue &optValue)
{
    mutexQueueFinished.lock();
    queueFinished.push(optValue);
    mutexQueueFinished.unlock();
}

OptValue OptBase::pop_todo()
{
    mutexQueueTodo.lock();
    auto out = queueTodo.front();
    queueTodo.pop();
    mutexQueueTodo.unlock();
    return out;
}

OptValue OptBase::pop_calculated()
{
    mutexQueueCalculated.lock();
    auto out = queueCalculated.front();
    queueCalculated.pop();
    mutexQueueCalculated.unlock();
    return out;
}

OptValue OptBase::pop_finished()
{
    mutexQueueFinished.lock();
    auto out = queueFinished.front();
    queueFinished.pop();
    mutexQueueFinished.unlock();
    return out;
}

