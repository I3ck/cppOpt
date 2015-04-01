#ifndef OPTBASE_H
#define OPTBASE_H

#include <vector>
#include <list>
#include <set>
#include <mutex>
#include <queue>
#include <limits>
#include <thread>
#include <chrono>
#include <cmath>
#include <fstream>

#include "config.h"
#include "OptTarget.h"
#include "OptSolverBase.h"
#include "OptCalculation.h"
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
        mutexAvailabilityCheckTodo,
        mutexQueueCalculated,
        mutexFinishedCalculations,
        mutexPOptimisers,
        mutexLogFile;

    static std::queue< std::pair<OptCalculation, OptBase*> >
        queueTodo;

    static std::vector< std::pair<OptCalculation, OptBase*> >
        finishedCalculations;

    static std::set <OptBase*>
        pOptimisers;

    static bool
        loggingEnabled; //only set with one method and already locked when logging, no additional mutex required

    static std::string
        loggingDelimiter,
        loggingLineEnd;

    static std::ofstream
        logFile;

    static unsigned int
        waitTimeMs;

protected:
    std::vector<OptCalculation>
        previousCalculations;

    OptCalculation
        bestCalculation;

    const OptBoundaries
        optBoundaries;

    const unsigned int
        maxCalculations;

    const OptSolverBase*
        pCalculator;

    const OptTarget
        optTarget;

    OPT_T
        targetValue;

// METHODS ---------------------------------------------------------------------

public:
    OptBase(const OptBoundaries &optBoundaries,
            unsigned int maxCalculations,
            OptSolverBase* pCalculator,
            OptTarget optTarget = MINIMIZE,
            OPT_T targetValue = 0.0);

    ~OptBase();

    static void run_optimisations(unsigned int maxThreads = 1);

    static unsigned int number_optimisers();

    static bool enable_logging(const std::string &pathLogFile,
                               const OptBoundaries &optBoundaries,
                               const std::string &delimiter = " ",
                               const std::string &lineEnd = "\n");

    static void set_wait_time(unsigned int timeInMs);

    //targetValue won't be used when maximizing or minimizing
    static OptCalculation get_best_calculation(OptTarget optTarget, OPT_T targetValue);

    OptCalculation get_best_calculation() const;

protected:
    //targetValue won't be used when maximizing or minimizing
    static bool result_better(const OptCalculation &result, const OptCalculation &other, OptTarget optTarget, OPT_T targetValue);

    virtual OptCalculation get_next_value() = 0; //must be implemented by algorithm derived classes

    void add_finished_calculation(OptCalculation optCalculation);

    OPT_T bad_value() const;

    bool valid(const OptCalculation &optCalculation) const;

    static OPT_T random_factor();

private:
    static void threaded_work();

    static void push_todo(OptCalculation optCalculation, OptBase *pOptBase);
    static void push_finished(OptCalculation optCalculation, OptBase *pOptBase);

    static bool available_todo();

    static std::pair<OptCalculation, OptBase*> pop_todo();

    static void log(const OptCalculation &optCalculation);
};

} // namespace cppOpt

#endif // OPTBASE_H
