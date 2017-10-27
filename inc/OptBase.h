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
#include <memory>
#include <optional>

#include "config.h"
#include "OptTarget.h"
#include "OptCalculation.h"
#include "OptBoundary.h"
#include "OptBoundaries.h"

#ifdef DEBUG
    #include <iostream>
#endif


namespace cppOpt
{

template <typename T>
class IOptAlgorithm ///@todo own file
{
public:
    virtual OptCalculation<T> get_next_calculation(
        vector<OptCalculation<T>> const& previous,
        OptCalculation<T>         const* best,
        OptBoundaries<T>          const& boundaries) = 0; ///@todo rename?

    virtual ~IOptAlgorithm() = 0;
};

template <typename T>
class OptAlgorithmBase : public IOptAlgorithm<T> ///@todo own file
{
public:
    virtual OptCalculation<T> get_next_calculation(
        vector<OptCalculation<T>> const& previous,
        OptCalculation<T>         const* best,
        OptBoundaries<T>          const& boundaries) = 0; ///@todo rename?
protected:
    static T random_factor()
    {
        return rand()/(T)(RAND_MAX);
    }

//------------------------------------------------------------------------------

    static OptCalculation<T> random_calculation(OptBoundaries<T> const& boundaries)
    {
        OptCalculation<T> optCalculation;
        for(auto const& boundary : boundaries)
        {
            T newValue = boundary.second.min + random_factor() * boundary.second.range();
            optCalculation.add_parameter(boundary.second.name, newValue);
        }
        return optCalculation;
    }

//------------------------------------------------------------------------------

    static T calculate_random_change(const OptBoundary<T> &boundary, T temperature)
    {
        T change, maxChange;

        maxChange = (T)0.5 * boundary.range() * temperature;
        change = random_factor() * maxChange;

        if(rand() % 2)
            change *= -1.0;

        return change;
    }

//------------------------------------------------------------------------------

    static bool valid(const OptCalculation<T> &optCalculation, OptBoundaries<T> const& boundaries)
    {
        for(auto const& boundary : boundaries)
        {
            if(   optCalculation.get_parameter(boundary.first) < boundary.second.min
               || optCalculation.get_parameter(boundary.first) > boundary.second.max)
               return false;
        }
        return true;
    }
};

using namespace std;

template <typename T>
using calc_t = function<void(OptCalculation<T>&)>; ///@todo own file?

using lock = lock_guard<recursive_mutex>;

template <typename T, bool isMultiThreaded = true>
class OptCoordinator
{
    using self = OptCoordinator<T, isMultiThreaded>;

private:
    recursive_mutex
        m;

    queue< pair<OptCalculation<T>, IOptAlgorithm<T>*> > ///@todo consider vector
        queueTodo;

    vector< pair<OptCalculation<T>, self*> >
        finishedCalculations;

    vector<unique_ptr<IOptAlgorithm<T>>>
        children;

    bool
        abortEarly{false};

    T
        abortValue{0};

    ///@todo logging can be done by simply passing a wrapping lambda (write helper for this?)
    const calc_t<T>
        calcFunction;

    const OptTarget
        optTarget;

    T
        targetValue;

protected: ///@todo these must be stored per-algorithm
    vector< OptCalculation<T> >
        previousCalculations;

    OptCalculation<T>
        bestCalculation;

    const unsigned int
        maxCalculations; ///@todo can be dropped here / stored globally?

    const OptBoundaries<T>
        optBoundaries;

//------------------------------------------------------------------------------

public:
    OptCoordinator(
        OptBoundaries<T> optBoundaries,
        unsigned int maxCalculations,
        calc_t<T> calcFunction,
        OptTarget optTarget,
        T targetValue) :

        maxCalculations(maxCalculations),
        optBoundaries(move(optBoundaries)),
        calcFunction(move(calcFunction)),
        optTarget(move(optTarget)),
        targetValue(move(targetValue))
    {
        previousCalculations.reserve(maxCalculations);
    }

    ///@todo needs method to add child (unique_ptr &&)

//------------------------------------------------------------------------------

    void add_child(unique_ptr<IOptAlgorithm<T>> child)
    {
        children.push_back(move(child));
    }

//------------------------------------------------------------------------------

    void run_optimisations(unsigned int maxThreads = 1) ///@otod drop the s?
    {
        run_optimisations(maxThreads, time(NULL));
    }

    void run_optimisations(unsigned int maxThreads, unsigned int randomSeed)
    {
        ///@todo call reset?
        srand(randomSeed);

        //get the first to-calculate value of every optimiser
        //and push it onto the todo queue
        {
            auto lck = lock_for();
            for(const auto &child : children)
                push_todo(child->get_next_calculation(std::vector<OptCalculation<T>>(), nullptr, optBoundaries), child.get()); ///@todo later pass previous?
        }

        if constexpr (isMultiThreaded) {
            vector <thread> threads;

            for(unsigned int i=0; i<maxThreads; ++i)
                threads.emplace_back([this](){return threaded_work();});

            for(auto &thread :threads)
                thread.join();
        } else
            threaded_work(); ///@todo rename function
    }

//------------------------------------------------------------------------------

    void clear_results() ///@todo still offer this?
    {
        auto lck = lock_for(); ///@todo might not be required in later flow (see comment below)
        finishedCalculations.clear();
    }

//------------------------------------------------------------------------------

    unsigned int number_optimisers() ///@todo rename
    {
        auto lck = lock_for();
        return children.size(); ///@todo unlikely this has to be locked (only access via the optimisers should be locked)
    }

//------------------------------------------------------------------------------

    void enable_early_abort(const T abortVal)
    {
        abortEarly = true;
        abortValue = abortVal;
    }

//------------------------------------------------------------------------------

    unsigned int number_finished_calculations()
    {
        auto lck = lock_for();
        return finishedCalculations.size();
    }

//------------------------------------------------------------------------------

    //targetValue won't be used when maximizing or minimizing
    OptCalculation<T> get_best_calculation(OptTarget optTarget, T targetValue) ///@todo rename to of all, drop params
    {
        auto lck = lock_for();
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
    static bool result_better(const OptCalculation<T> &result, const OptCalculation<T> &other, OptTarget optTarget, T targetValue) ///@todo consider implementing this in OptCalculation
    {
        switch(optTarget)
        {
            case OptTarget::MINIMIZE:
                return result.result < other.result;

            case OptTarget::MAXIMIZE:
                return result.result > other.result;

            case OptTarget::APPROACH:
                return fabs(targetValue - result.result) < fabs(targetValue - other.result);

            case OptTarget::DIVERGE:
                return fabs(targetValue - result.result) > fabs(targetValue - other.result);

            default: //MINIMIZE
                return result.result < other.result;
        }
    }
//------------------------------------------------------------------------------

    void add_finished_calculation(OptCalculation<T> optCalculation) ///@todo does too much?
    {
        previousCalculations.push_back(optCalculation);

        {
            auto lck = lock_for();
            finishedCalculations.push_back({optCalculation, this});
        }

        if(result_better(optCalculation, bestCalculation, optTarget, targetValue))
            bestCalculation = optCalculation;
    }

//------------------------------------------------------------------------------

    unsigned int index_closest_calculation(const vector< OptCalculation<T> > &optCalculations, unsigned int indexThis) const ///@todo could be moved to some helper, also should be easy to simplify heavily
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

private:
    inline auto lock_for() {
        if constexpr (isMultiThreaded) {
            return make_optional<lock>(m);
        } else
            return nullopt;
    }

//------------------------------------------------------------------------------

    void threaded_work() ///@todo rename
    {
        while(true)
        {
            bool availableTodo(false);
            pair <OptCalculation<T>, IOptAlgorithm<T>*> todo;
            {
                auto lck = lock_for();
                availableTodo = available_todo();
                if(availableTodo)
                    todo = pop_todo();
            }

            if(!availableTodo)
                break;

            //OptCalculation<T> optCalculation = todo.first;
            //OptBase* pOptBase = todo.second;

            //pOptBase->calcFunction(optCalculation);

            //pOptBase->add_finished_calculation(optCalculation);

            //if(pOptBase->previousCalculations.size() >= pOptBase->maxCalculations)
            //    break;

            if(abortEarly)
            {
                OptCalculation<T> calcEarly;
                calcEarly.result = abortValue;

                //if(result_better(optCalculation, calcEarly, pOptBase->optTarget , pOptBase->targetValue))
                //    break;
            }

            //only add the next one if there still are more
            //push_todo(pOptBase->get_next_calculation(), pOptBase);
        }
    }

//------------------------------------------------------------------------------

    void push_todo(OptCalculation<T> optCalculation, IOptAlgorithm<T>* algo) ///@todo all the locking in the functions below could become unnecessary since only the thread callbacks have to use the guards (just lock once there)
    {
        auto lck = lock_for();
        queueTodo.push({optCalculation, algo});
    }

//------------------------------------------------------------------------------

    void push_finished(OptCalculation<T> optCalculation, self *pOptBase)
    {
        auto lck = lock_for();
        finishedCalculations.push_back({optCalculation, pOptBase});
    }

//------------------------------------------------------------------------------

    bool available_todo()
    {
        auto lck = lock_for();
        return !queueTodo.empty();
    }

//------------------------------------------------------------------------------

    pair<OptCalculation<T>, IOptAlgorithm<T>*> pop_todo()
    {
        auto lck = lock_for();
        auto out = queueTodo.front();
        queueTodo.pop();
        return out;
    }

//------------------------------------------------------------------------------

};

///@todo these can all be removed / initialized within the class since not static anymore


} // namespace cppOpt








/*
using namespace std;

template <typename T>
using calc_t = function<void(OptCalculation<T>&)>; ///@todo own file?

using lock = lock_guard<recursive_mutex>;

template <typename T, bool isMultiThreaded = true>
class OptBase
{
    using self = OptBase<T, isMultiThreaded>;

private:
    static recursive_mutex
        m;

    static queue< pair<OptCalculation<T>, self*> >
        queueTodo;

    static vector< pair<OptCalculation<T>, self*> >
        finishedCalculations;

    static set <OptBase*>
        pOptimisers;

    static bool
        loggingEnabled;

    static bool
        abortEarly;

    static T
        abortValue;

    static string
        loggingDelimiter,
        loggingLineEnd;

    static ofstream
        logFile;

protected:
    vector< OptCalculation<T> >
        previousCalculations;

    OptCalculation<T>
        bestCalculation;

    const unsigned int
        maxCalculations;

    const OptBoundaries<T>
        optBoundaries;

    const calc_t<T>
        calcFunction;

    const OptTarget
        optTarget;

    T
        targetValue;

//------------------------------------------------------------------------------

public:
    OptBase(
        OptBoundaries<T> optBoundaries,
        unsigned int maxCalculations,
        calc_t<T> calcFunction,
        OptTarget optTarget,
        T targetValue) :

        maxCalculations(maxCalculations),
        optBoundaries(move(optBoundaries)),
        calcFunction(move(calcFunction)),
        optTarget(move(optTarget)),
        targetValue(move(targetValue))
    {
        previousCalculations.reserve(maxCalculations);
        auto lck = lock_for();
        pOptimisers.insert(this);
    }

//------------------------------------------------------------------------------

    virtual ~OptBase()
    {
        auto lck = lock_for();
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
            auto lck = lock_for();
            for(const auto &pOptimiser : pOptimisers)
            {
                if(pOptimiser->previousCalculations.size() == 0)
                    push_todo(pOptimiser->get_next_calculation(), pOptimiser);
            }
        }

        if constexpr (isMultiThreaded) {
            vector <thread> threads;

            for(unsigned int i=0; i<maxThreads; ++i)
                threads.emplace_back(threaded_work);

            for(auto &thread :threads)
                thread.join();
        } else
            threaded_work(); ///@todo rename function
    }

//------------------------------------------------------------------------------

    static void clear_results()
    {
        auto lck = lock_for();
        finishedCalculations.clear();
    }

//------------------------------------------------------------------------------

    static unsigned int number_optimisers()
    {
        auto lck = lock_for();
        return pOptimisers.size();
    }

//------------------------------------------------------------------------------

    static bool enable_logging(const string &pathLogFile,
                               const OptBoundaries<T> &optBoundaries,
                               const string &delimiter = " ",
                               const string &lineEnd = "\n")
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
        auto lck = lock_for();
        return finishedCalculations.size();
    }

//------------------------------------------------------------------------------

    //targetValue won't be used when maximizing or minimizing
    static OptCalculation<T> get_best_calculation(OptTarget optTarget, T targetValue)
    {
        auto lck = lock_for();
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
            case OptTarget::MINIMIZE:
                return result.result < other.result;

            case OptTarget::MAXIMIZE:
                return result.result > other.result;

            case OptTarget::APPROACH:
                return fabs(targetValue - result.result) < fabs(targetValue - other.result);

            case OptTarget::DIVERGE:
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
            auto lck = lock_for();
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
            case OptTarget::MINIMIZE:
                return numeric_limits<T>::max();

            case OptTarget::MAXIMIZE:
                return numeric_limits<T>::lowest();

            case OptTarget::APPROACH:
                if(targetValue > 0.0)
                    return numeric_limits<T>::lowest();
                else
                    return numeric_limits<T>::max();

            case OptTarget::DIVERGE:
                return targetValue;

            default: //MINIMIZE
                return numeric_limits<T>::max();
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

    unsigned int index_closest_calculation(const vector< OptCalculation<T> > &optCalculations, unsigned int indexThis) const
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
    static inline auto lock_for() {
        if constexpr (isMultiThreaded) {
            return make_optional<lock>(m);
        } else
            return nullopt;
    }

//------------------------------------------------------------------------------

    static void threaded_work()
    {
        while(true)
        {
            bool availableTodo(false);
            pair <OptCalculation<T>, self*> todo;
            {
                auto lck = lock_for();
                availableTodo = available_todo();
                if(availableTodo)
                    todo = pop_todo();
            }

            if(!availableTodo)
                break;

            OptCalculation<T> optCalculation = todo.first;
            OptBase* pOptBase = todo.second;

            pOptBase->calcFunction(optCalculation);

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

    static void push_todo(OptCalculation<T> optCalculation, self *pOptBase)
    {
        auto lck = lock_for();
        queueTodo.push({optCalculation, pOptBase});
    }

//------------------------------------------------------------------------------

    static void push_finished(OptCalculation<T> optCalculation, self *pOptBase)
    {
        auto lck = lock_for();
        finishedCalculations.push_back({optCalculation, pOptBase});
    }

//------------------------------------------------------------------------------

    static bool available_todo()
    {
        auto lck = lock_for();
        return !queueTodo.empty();
    }

//------------------------------------------------------------------------------

    static pair<OptCalculation<T>, self*> pop_todo()
    {
        auto lck = lock_for();
        auto out = queueTodo.front();
        queueTodo.pop();
        return out;
    }

//------------------------------------------------------------------------------

    static void log(const OptCalculation<T> &optCalculation)
    {
        auto lck = lock_for();
        logFile << optCalculation.to_string_values(loggingDelimiter) << loggingLineEnd;
    }

//------------------------------------------------------------------------------

};

template <typename T, bool isMultiThreaded>
recursive_mutex OptBase<T, isMultiThreaded>::m;

template <typename T, bool isMultiThreaded>
queue< pair<OptCalculation<T>, OptBase<T, isMultiThreaded>*> >
    OptBase<T, isMultiThreaded>::queueTodo;

template <typename T, bool isMultiThreaded>
vector< pair<OptCalculation<T>, OptBase<T, isMultiThreaded>*> >
    OptBase<T, isMultiThreaded>::finishedCalculations;

template <typename T, bool isMultiThreaded>
set<OptBase<T, isMultiThreaded>*>
    OptBase<T, isMultiThreaded>::pOptimisers;

template <typename T, bool isMultiThreaded>
bool
    OptBase<T, isMultiThreaded>::loggingEnabled(false);

template <typename T, bool isMultiThreaded>
bool
    OptBase<T, isMultiThreaded>::abortEarly(false);

template <typename T, bool isMultiThreaded>
T
    OptBase<T, isMultiThreaded>::abortValue(0);

template <typename T, bool isMultiThreaded>
string OptBase<T, isMultiThreaded>::loggingDelimiter("");

template <typename T, bool isMultiThreaded>
string OptBase<T, isMultiThreaded>::loggingLineEnd("");

template <typename T, bool isMultiThreaded>
ofstream
    OptBase<T, isMultiThreaded>::logFile;

} // namespace cppOpt
*/
#endif // OPTBASE_H
