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

    static std::string
        loggingDelimiter,
        loggingLineEnd;

    static std::ofstream
        logFile;

    static unsigned int
        waitTimeMs;

protected:
    std::vector< OptCalculation<T> >
        previousCalculations;

    OptCalculation<T>
        bestCalculation;

    const OptBoundaries<T>
        optBoundaries;

    const unsigned int
        maxCalculations;

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
        mutexPOptimisers.lock();
        pOptimisers.insert(this);
        mutexPOptimisers.unlock();
        srand( time(NULL) + rand() );
    }

//------------------------------------------------------------------------------

    ~OptBase()
    {
        mutexPOptimisers.lock();
        pOptimisers.erase( pOptimisers.find(this) );
        mutexPOptimisers.unlock();
    }

//------------------------------------------------------------------------------

    static void run_optimisations(unsigned int maxThreads = 1)
    {
        //get the first to-calculate value of every optimiser
        //and push it onto the todo queue
        mutexPOptimisers.lock();
        for(const auto &pOptimiser : pOptimisers)
        {
            if(pOptimiser->previousCalculations.size() == 0)
                push_todo(pOptimiser->get_next_calculation(), pOptimiser);
        }
        mutexPOptimisers.unlock();

        std::vector <std::thread> threads;

        for(unsigned int i=0; i<maxThreads; ++i)
            threads.emplace_back(  std::thread( std::bind(&OptBase::threaded_work) )  );

        for(auto &thread :threads)
            thread.join();

        ///@todo close logfile?
    }

//------------------------------------------------------------------------------

    static unsigned int number_optimisers()
    {
        unsigned int out(0);
        mutexPOptimisers.lock();
        out = pOptimisers.size();
        mutexPOptimisers.unlock();
        return out;
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

    static void set_wait_time(unsigned int timeInMs)
    {
        waitTimeMs = timeInMs;
    }

//------------------------------------------------------------------------------

    static unsigned int number_finished_calculations()
    {
        mutexFinishedCalculations.lock();
        unsigned int nFinishedCalculations = finishedCalculations.size();
        mutexFinishedCalculations.unlock();

        return nFinishedCalculations;
    }

//------------------------------------------------------------------------------

    //targetValue won't be used when maximizing or minimizing
    static OptCalculation<T> get_best_calculation(OptTarget optTarget, T targetValue)
    {
        OptCalculation<T> out;
        mutexFinishedCalculations.lock();

        if(finishedCalculations.size() == 0)
        {
            mutexFinishedCalculations.unlock();
            return out;
        }

        out = finishedCalculations[0].first;

        for(const auto &finishedCalculation : finishedCalculations)
            if(result_better(finishedCalculation.first, out, optTarget, targetValue))
                out = finishedCalculation.first;

        mutexFinishedCalculations.unlock();
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

        mutexFinishedCalculations.lock();
        finishedCalculations.push_back({optCalculation, this});
        mutexFinishedCalculations.unlock();

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

//------------------------------------------------------------------------------

    OptCalculation<T> random_calculation() const
    {
        OptCalculation<T> optCalculation;
        for(auto boundary = optBoundaries.cbegin(); boundary != optBoundaries.cend(); ++boundary)
        {
            T newValue = boundary->min + random_factor() * boundary->range();
            optCalculation.add_parameter(boundary->name, newValue);
        }
        return optCalculation;
    }

//------------------------------------------------------------------------------

    bool valid(const OptCalculation<T> &optCalculation) const
    {
        for(auto boundary = optBoundaries.cbegin(); boundary != optBoundaries.cend(); ++boundary)
        {
            if(
            optCalculation.get_parameter(boundary->name) < boundary->min
            ||
            optCalculation.get_parameter(boundary->name) > boundary->max)
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

        maxChange = 0.5 * boundary.range() * temperature;
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
            mutexAvailabilityCheckTodo.lock(); //the check for availability and the pop have to be atomic
            bool availableTodo = available_todo();

            if(!availableTodo)
                mutexAvailabilityCheckTodo.unlock(); //no pop will occur, can unlock directly

            else // availableTodo
            {
                std::pair <OptCalculation<T>, OptBase<T>*> todo = pop_todo();
                mutexAvailabilityCheckTodo.unlock(); //pop occured, can unlock now
                OptCalculation<T> optCalculation = todo.first;
                OptBase* pOptBase = todo.second;

                pOptBase->pCalculator->calculate(optCalculation);

                if(loggingEnabled)
                    log(optCalculation);

                pOptBase->add_finished_calculation(optCalculation);

                if(pOptBase->previousCalculations.size() >= pOptBase->maxCalculations)
                    break;

                //only add the next one if there still are more
                push_todo(pOptBase->get_next_calculation(), pOptBase);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(waitTimeMs));
        }

    }

//------------------------------------------------------------------------------

    static void push_todo(OptCalculation<T> optCalculation, OptBase<T> *pOptBase)
    {
        mutexQueueTodo.lock();
        queueTodo.push({optCalculation, pOptBase});
        mutexQueueTodo.unlock();
    }

//------------------------------------------------------------------------------

    static void push_finished(OptCalculation<T> optCalculation, OptBase<T> *pOptBase)
    {
        mutexFinishedCalculations.lock();
        finishedCalculations.push_back({optCalculation, pOptBase});
        mutexFinishedCalculations.unlock();
    }

//------------------------------------------------------------------------------

    static bool available_todo()
    {
        bool out(false);

        mutexQueueTodo.lock();
        out = !queueTodo.empty();
        mutexQueueTodo.unlock();

        return out;
    }

//------------------------------------------------------------------------------

    static std::pair<OptCalculation<T>, OptBase<T>*> pop_todo()
    {
        mutexQueueTodo.lock();
        auto out = queueTodo.front();
        queueTodo.pop();
        mutexQueueTodo.unlock();
        return out;
    }

//------------------------------------------------------------------------------

    static void log(const OptCalculation<T> &optCalculation)
    {
        mutexLogFile.lock();
        logFile << optCalculation.to_string_values(loggingDelimiter) << loggingLineEnd;
        mutexLogFile.unlock();
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
std::string OptBase<T>::loggingDelimiter("");

template <typename T>
std::string OptBase<T>::loggingLineEnd("");

template <typename T>
std::ofstream
    OptBase<T>::logFile;

template <typename T>
unsigned int
    OptBase<T>::waitTimeMs(0);

} // namespace cppOpt

#endif // OPTBASE_H
