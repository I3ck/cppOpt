#ifndef OPTBASE_H
#define OPTBASE_H

#include <vector>
#include <mutex>
#include <queue>

#include "defines.h"
#include "OptValue.h"


class OptBase
{
private:
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



    virtual OptValue get_next_value() = 0;

public:
    OptBase(const std::vector<OptValue> &optValues);
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
