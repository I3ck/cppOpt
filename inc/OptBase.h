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
