#ifndef OPTBASE_H
#define OPTBASE_H

#include <vector> ///@todo may not be needed anymore
#include <set>
#include <mutex>
#include <queue>
#include <limits>
#include <thread>
#include <chrono>
#include <cmath>
#include <fstream>

#include "defines.h"
#include "CalculatorBase.h"
#include "OptValue.h"
#include "OptBoundary.h"
#include "OptBoundaries.h"

#ifdef DEBUG
    #include <iostream>
#endif

namespace cppOpt
{

class OptBase
{

// MEMBERS ---------------------------------------------------------------------

private:
    static std::mutex
        mutexQueueTodo,
        mutexQueueCalculated,
        mutexQueueFinished,
        mutexPOptimizers,
        mutexLogFile;

    static std::queue< std::pair<OptValue, OptBase*> >
        queueTodo,
        queueFinished; ///@todo maybe use vector here

    static std::set <OptBase*>
        pOptimizers; ///@todo find better name (also update mutex name)

    static bool
        loggingEnabled; //only set with one method, no mutex required

    static std::ofstream
        logFile;

protected:
    std::vector<OptValue>
        previousCalculations;

    OptValue
        bestCalculation;

    const OptBoundaries
        optBoundaries;

    const unsigned int
        maxCalculations;

    const CalculatorBase*
        pCalculator;

    const OptTarget
        optTarget;

    T
        targetValue;

// METHODS ---------------------------------------------------------------------

public:
    OptBase(const OptBoundaries &optBoundaries,
            unsigned int maxCalculations,
            CalculatorBase* pCalculator,
            OptTarget optTarget = MINIMIZE,
            T targetValue = 0.0);

    ~OptBase();

    static void run_optimisations(unsigned int maxThreads);
    static unsigned int number_optimizers();
    static bool enable_logging(const std::string &pathLogFile, const OptBoundaries &optBoundaries);

protected:
    virtual OptValue get_next_value() = 0;

    void add_finished_calculation(OptValue optValue);

    T bad_value() const;

    bool result_better(const OptValue &result, const OptValue &other) const;

    static T random_factor();

private:
    static void threaded_work();

    static void push_todo(OptValue optValue, OptBase *pOptBase);
    static void push_finished(OptValue optValue, OptBase *pOptBase);

    static bool available_todo();
    static bool available_finished();

    static std::pair<OptValue, OptBase*> pop_todo();
    static std::pair<OptValue, OptBase*> pop_finished();

    static void log(const OptValue &optValue);
};

} // namespace cppOpt

#endif // OPTBASE_H
