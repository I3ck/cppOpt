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
#include <set>
#include <mutex>
#include <queue>
#include <limits>
#include <thread>
#include <chrono>
#include <cmath>
#include <fstream>
#include <functional>

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
template <typename T>
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

    static std::queue< std::pair<OptCalculation<T>, OptBase<T>*> >
        queueTodo;

    static std::vector< std::pair<OptCalculation<T>, OptBase<T>*> >
        finishedCalculations;

    static std::set <OptBase*>
        pOptimisers;

    static bool
        loggingEnabled; //only set with one method and already locked when logging, no additional mutex required

    static std::atomic<bool>
        abortEarly;

    static std::atomic<T>
        abortValue;

    static std::string
        loggingDelimiter,
        loggingLineEnd;

    static std::ofstream
        logFile;

protected:
    std::vector< OptCalculation<T> >
        previousCalculations;

    OptCalculation<T>
        bestCalculation;

    const unsigned int
        maxCalculations;

    const OptBoundaries<T>
        optBoundaries;

    const OptSolverBase<T>*
        pCalculator;

    const OptTarget
        optTarget;

    T
        targetValue;

// METHODS ---------------------------------------------------------------------

public:
    OptBase(const OptBoundaries<T> &optBoundaries,
            unsigned int maxCalculations,
            OptSolverBase<T>* pCalculator,
            OptTarget optTarget,
            T targetValue) :
        maxCalculations(maxCalculations),
        optBoundaries(optBoundaries),
        pCalculator(pCalculator),
        optTarget(optTarget),
        targetValue(targetValue)
    {
        previousCalculations.reserve(maxCalculations);

        std::lock_guard<std::mutex> lck(mutexPOptimisers);
        pOptimisers.insert(this);
    }

//------------------------------------------------------------------------------

    ~OptBase()
    {
        std::lock_guard<std::mutex> lck(mutexPOptimisers);
        pOptimisers.erase( pOptimisers.find(this) );
    }

//------------------------------------------------------------------------------

    static void run_optimisations(unsigned int maxThreads = 1)
    {
        run_optimisations(maxThreads, time(NULL));
    }

    static void run_optimisations(unsigned int maxThreads, unsigned int randomSeed)
    {
        srand(randomSeed);

        //get the first to-calculate value of every optimiser
        //and push it onto the todo queue
        {
            std::lock_guard<std::mutex> lck(mutexPOptimisers);
            for(const auto &pOptimiser : pOptimisers)
            {
                if(pOptimiser->previousCalculations.size() == 0)
                    push_todo(pOptimiser->get_next_calculation(), pOptimiser);
            }
        }

        std::vector <std::thread> threads;

        for(unsigned int i=0; i<maxThreads; ++i)
            threads.emplace_back(  std::thread( std::bind(&OptBase::threaded_work) )  );

        for(auto &thread :threads)
            thread.join();
    }

//------------------------------------------------------------------------------

    static void clear_results()
    {
        std::lock_guard<std::mutex> lck(mutexFinishedCalculations);
        finishedCalculations.clear();
    }

//------------------------------------------------------------------------------

    static unsigned int number_optimisers()
    {
        std::lock_guard<std::mutex> lck(mutexPOptimisers);
        return pOptimisers.size();
    }

//------------------------------------------------------------------------------

    static bool enable_logging(const std::string &pathLogFile,
                               const OptBoundaries<T> &optBoundaries,
                               const std::string &delimiter = " ",
                               const std::string &lineEnd = "\n")
    {
        logFile.open(pathLogFile);
        if(logFile.fail())
            return false;
        loggingEnabled = true;
        loggingDelimiter = delimiter;
        loggingLineEnd = lineEnd;
        logFile << optBoundaries.to_string() << "RESULT" << loggingLineEnd;
        return true;
    }

//------------------------------------------------------------------------------

    static void enable_early_abort(const T abortVal)
    {
        abortEarly = true;
        abortValue = abortVal;
    }

//------------------------------------------------------------------------------

    static unsigned int number_finished_calculations()
    {
        std::lock_guard<std::mutex> lck(mutexFinishedCalculations);
        return finishedCalculations.size();
    }

//------------------------------------------------------------------------------

    //targetValue won't be used when maximizing or minimizing
    static OptCalculation<T> get_best_calculation(OptTarget optTarget, T targetValue)
    {
        std::lock_guard<std::mutex> lck(mutexFinishedCalculations);
        OptCalculation<T> out;

        if(finishedCalculations.size() == 0)
            return out;

        out = finishedCalculations[0].first;

        for(const auto &finishedCalculation : finishedCalculations)
            if(result_better(finishedCalculation.first, out, optTarget, targetValue))
                out = finishedCalculation.first;

        return out;
    }

//------------------------------------------------------------------------------

    OptCalculation<T> get_best_calculation() const
    {
       return bestCalculation;
    }

//------------------------------------------------------------------------------

    unsigned int number_previous_calculations() const
    {
        return previousCalculations.size();
    }

//------------------------------------------------------------------------------


protected:
    //targetValue won't be used when maximizing or minimizing
    static bool result_better(const OptCalculation<T> &result, const OptCalculation<T> &other, OptTarget optTarget, T targetValue)
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

    virtual OptCalculation<T> get_next_calculation() = 0; //must be implemented by algorithm derived classes

//------------------------------------------------------------------------------

    void add_finished_calculation(OptCalculation<T> optCalculation)
    {
        previousCalculations.push_back(optCalculation);

        {
            std::lock_guard<std::mutex> lck(mutexFinishedCalculations);
            finishedCalculations.push_back({optCalculation, this});
        }

        if(result_better(optCalculation, bestCalculation, optTarget, targetValue))
            bestCalculation = optCalculation;
    }

//------------------------------------------------------------------------------

    T bad_value() const
    {
        switch(optTarget)
        {
            case MINIMIZE:
                return std::numeric_limits<T>::max();

            case MAXIMIZE:
                return std::numeric_limits<T>::lowest();

            case APPROACH:
                if(targetValue > 0.0)
                    return std::numeric_limits<T>::lowest();
                else
                    return std::numeric_limits<T>::max();

            case DIVERGE:
                return targetValue;

            default: //MINIMIZE
                return std::numeric_limits<T>::max();
        }
    }

//------------------------------------------------------------------------------

    OptCalculation<T> random_calculation() const
    {
        OptCalculation<T> optCalculation;
        for(auto boundary = optBoundaries.cbegin(); boundary != optBoundaries.cend(); ++boundary)
        {
            T newValue = boundary->second.min + random_factor() * boundary->second.range();
            optCalculation.add_parameter(boundary->second.name, newValue);
        }
        return optCalculation;
    }

//------------------------------------------------------------------------------

    bool valid(const OptCalculation<T> &optCalculation) const
    {
        for(auto boundary = optBoundaries.cbegin(); boundary != optBoundaries.cend(); ++boundary)
        {
            if(
            optCalculation.get_parameter(boundary->first) < boundary->second.min
            ||
            optCalculation.get_parameter(boundary->first) > boundary->second.max)
                return false;
        }
        return true;
    }

//------------------------------------------------------------------------------

    static T random_factor()
    {
        return rand()/(T)(RAND_MAX);
    }

//------------------------------------------------------------------------------

    OptCalculation<T> random_start_value()
    {
        OptCalculation<T> optCalculation = random_calculation();
        bestCalculation = optCalculation;
        bestCalculation.result = bad_value();
        return optCalculation;
    }

//------------------------------------------------------------------------------

    unsigned int index_closest_calculation(const std::vector< OptCalculation<T> > &optCalculations, unsigned int indexThis) const
    {
        T closestDistance;
        unsigned int indexClosest(0);
        bool initialised(false);

        for(unsigned int i = 0; i < optCalculations.size(); ++i)
        {
            if(i == indexThis)
                continue;

            if(!initialised)
            {
                closestDistance = optCalculations[i].distance_to(optCalculations[indexThis]);
                indexClosest = i;
                initialised = true;
            }
            else
            {
                T distance = optCalculations[i].distance_to(optCalculations[indexThis]);

                if(distance < closestDistance)
                {
                    closestDistance = distance;
                    indexClosest = i;
                }
            }
        }
        return indexClosest;
    }

//------------------------------------------------------------------------------

    T calculate_random_change(const OptBoundary<T> &boundary, T temperature) const
    {
        T change, maxChange;

        maxChange = (T)0.5 * boundary.range() * temperature;
        change = random_factor() * maxChange;

        if(rand() % 2)
            change *= -1.0;

        return change;
    }

//------------------------------------------------------------------------------

private:
    static void threaded_work()
    {
        while(true)
        {
            bool availableTodo(false);
            std::pair <OptCalculation<T>, OptBase<T>*> todo;
            {
                std::lock_guard<std::mutex> lck(mutexAvailabilityCheckTodo); //the check for availability and the pop have to be atomic
                availableTodo = available_todo();
                if(availableTodo)
                    todo = pop_todo();
            }

            if(!availableTodo)
                break;

            OptCalculation<T> optCalculation = todo.first;
            OptBase* pOptBase = todo.second;

            pOptBase->pCalculator->calculate(optCalculation);

            if(loggingEnabled)
                log(optCalculation);

            pOptBase->add_finished_calculation(optCalculation);

            if(pOptBase->previousCalculations.size() >= pOptBase->maxCalculations)
                break;

            if(abortEarly)
            {
                OptCalculation<T> calcEarly;
                calcEarly.result = abortValue;

                if(result_better(optCalculation, calcEarly, pOptBase->optTarget , pOptBase->targetValue))
                    break;
            }

            //only add the next one if there still are more
            push_todo(pOptBase->get_next_calculation(), pOptBase);
        }
    }

//------------------------------------------------------------------------------

    static void push_todo(OptCalculation<T> optCalculation, OptBase<T> *pOptBase)
    {
        std::lock_guard<std::mutex> lck(mutexQueueTodo);
        queueTodo.push({optCalculation, pOptBase});
    }

//------------------------------------------------------------------------------

    static void push_finished(OptCalculation<T> optCalculation, OptBase<T> *pOptBase)
    {
        std::lock_guard<std::mutex> lck(mutexFinishedCalculations);
        finishedCalculations.push_back({optCalculation, pOptBase});
    }

//------------------------------------------------------------------------------

    static bool available_todo()
    {
        std::lock_guard<std::mutex> lck(mutexQueueTodo);
        return !queueTodo.empty();
    }

//------------------------------------------------------------------------------

    static std::pair<OptCalculation<T>, OptBase<T>*> pop_todo()
    {
        std::lock_guard<std::mutex> lck(mutexQueueTodo);
        auto out = queueTodo.front();
        queueTodo.pop();
        return out;
    }

//------------------------------------------------------------------------------

    static void log(const OptCalculation<T> &optCalculation)
    {
        std::lock_guard<std::mutex> lck(mutexLogFile);
        logFile << optCalculation.to_string_values(loggingDelimiter) << loggingLineEnd;
    }

//------------------------------------------------------------------------------

};

template <typename T>
std::mutex OptBase<T>::mutexQueueTodo;

template <typename T>
std::mutex OptBase<T>::mutexAvailabilityCheckTodo;

template <typename T>
std::mutex OptBase<T>::mutexQueueCalculated;

template <typename T>
std::mutex OptBase<T>::mutexFinishedCalculations;

template <typename T>
std::mutex OptBase<T>::mutexPOptimisers;

template <typename T>
std::mutex OptBase<T>::mutexLogFile;

template <typename T>
std::queue< std::pair<OptCalculation<T>, OptBase<T>*> >
    OptBase<T>::queueTodo;

template <typename T>
std::vector< std::pair<OptCalculation<T>, OptBase<T>*> >
    OptBase<T>::finishedCalculations;

template <typename T>
std::set<OptBase<T>*>
    OptBase<T>::pOptimisers;

template <typename T>
bool
    OptBase<T>::loggingEnabled(false);

template <typename T>
std::atomic<bool>
    OptBase<T>::abortEarly(false);

template <typename T>
std::atomic<T>
    OptBase<T>::abortValue(0);

template <typename T>
std::string OptBase<T>::loggingDelimiter("");

template <typename T>
std::string OptBase<T>::loggingLineEnd("");

template <typename T>
std::ofstream
    OptBase<T>::logFile;

} // namespace cppOpt

#endif // OPTBASE_H
