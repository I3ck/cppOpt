#ifndef OPTBASE_H
#define OPTBASE_H

#include <vector>
#include <mutex>
#include <queue>
#include <limits>
#include <thread>
#include <chrono>
#include <cmath>

#include "defines.h"
#include "CalculatorBase.h"
#include "OptValue.h"
#include "OptBoundary.h"

#ifdef DEBUG
    #include <iostream>
#endif

//class OptValue; //forward reference


class OptBase
{
private:
    ///@todo all static members need a mutex
    static std::mutex
        mutexQueueTodo,
        mutexQueueCalculated,
        mutexQueueFinished,
        mutexPOptimizers;

    static std::queue< std::pair<OptValue, OptBase*> >
        queueTodo,
        queueFinished; ///@todo maybe use vector here

    static std::vector <OptBase*>
        pOptimizers; ///@todo find better name (also update mutex name)

protected:

    std::vector<OptValue>
        previousCalculations;

    OptValue
        bestCalculation;

    const std::vector<OptBoundary>
        optBoundaries;

    const unsigned int
        maxCalculations;

    const CalculatorBase*
        pCalculator;

    const OptTarget
        optTarget;

    T
        targetValue;



    virtual OptValue get_next_value() = 0;

public:
    ///@todo OptValues should be OptBoundaries
    OptBase(const std::vector<OptBoundary> &optBoundaries,
            unsigned int maxCalculations,
            CalculatorBase* pCalculator,
            OptTarget optTarget = MINIMIZE,
            T targetValue = 0.0);
    ~OptBase();


    void add_finished_calculation(OptValue optValue, OptBase *pOptBase);

    T bad_value() const;

    bool result_better(const OptValue &result, const OptValue &other) const;




    static void run_optimisations(unsigned int maxThreads);
    static T random_factor();
    ///@todo some / all of these should maybe be made private / protected
    static void threaded_work();
    static void push_todo(OptValue optValue, OptBase *pOptBase);
    static void push_finished(OptValue optValue, OptBase *pOptBase);

    static bool available_todo();
    static bool available_finished();

    static std::pair<OptValue, OptBase*> pop_todo();
    static std::pair<OptValue, OptBase*> pop_finished();
};

#endif // OPTBASE_H
