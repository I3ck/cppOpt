#ifndef OPTBASE_H
#define OPTBASE_H

#include <vector>
#include <mutex>
#include <queue>

#include "defines.h"
#include "OptValue.h"
#include "OptBoundary.h"

class OptValue; //forward reference


class OptBase
{
private:
    ///@todo alls static members need a mutex
    static std::mutex
        mutexQueueTodo,
        mutexQueueCalculated,
        mutexQueueFinished;

    static std::queue<OptValue>
        queueTodo,
        queueCalculated,
        queueFinished;

    std::mutex
        mutexPreviousCalculations;

    std::vector<OptValue>
        previousCalculations;

    const std::vector<OptBoundary>
        optBoundaries;

    const unsigned int
        maxCalculations;

    const OptTarget
        optTarget;



    virtual OptValue get_next_value() = 0;

public:
    ///@todo OptValues should be OptBoundaries
    OptBase(const std::vector<OptBoundary> &optBoundaries,
            unsigned int maxCalculations,
            OptTarget optTarget = MINIMIZE);
    ~OptBase();

    void add_finished_calculation(const OptValue &optValue);

    ///@todo some / all of these should maybe be made private / protected
    static void push_todo(const OptValue &optValue);
    static void push_calculated(const OptValue &optValue);
    static void push_finished(const OptValue &optValue);

    static OptValue pop_todo();
    static OptValue pop_calculated();
    static OptValue pop_finished();
};

#endif // OPTBASE_H
