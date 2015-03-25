#include "OptBase.h"

std::mutex
    OptBase::mutexQueueTodo,
    OptBase::mutexQueueCalculated,
    OptBase::mutexQueueFinished;

std::queue<OptValue>
    OptBase::queueTodo,
    OptBase::queueCalculated,
    OptBase::queueFinished;

OptBase::OptBase(const std::vector<OptValue> &optValues, OptTarget optTarget) :
    optTarget(optTarget)
{


}

OptBase::~OptBase()
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

