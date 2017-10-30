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

#ifndef OPTCOORDINATOR_H
#define OPTCOORDINATOR_H

#include <vector>
#include <set>
#include <mutex>
#include <queue>
#include <limits>
#include <thread>
#include <memory>
#include <optional>

#include "config.h"
#include "OptTarget.h"
#include "OptCalculation.h"
#include "OptBoundary.h"
#include "OptBoundaries.h"
#include "IOptAlgorithm.h"

namespace cppOpt
{

using namespace std;

using lock = lock_guard<recursive_mutex>;

template <typename T, bool isMultiThreaded = true>
class OptCoordinator final
{
    using self = OptCoordinator<T, isMultiThreaded>;

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

    const calc_t<T>
        calcFunction;

    const OptTarget
        optTarget;

    const T
        targetValue;

    map<IOptAlgorithm<T>*, std::vector<OptCalculation<T>>>
        previousCalculations;

    map<IOptAlgorithm<T>*, OptCalculation<T>>
        bestCalculations;

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

        calcFunction(move(calcFunction)),
        optTarget(move(optTarget)),
        targetValue(move(targetValue)),
        maxCalculations(maxCalculations),
        optBoundaries(move(optBoundaries))
    {}

//------------------------------------------------------------------------------

    void add_child(unique_ptr<IOptAlgorithm<T>> child)
    {
        children.push_back(move(child));
    }

//------------------------------------------------------------------------------

    void run_optimisation(unsigned int maxThreads = 1)
    {
        run_optimisation(maxThreads, time(NULL));
    }

    void run_optimisation(unsigned int maxThreads, unsigned int randomSeed)
    {
        srand(randomSeed);

        //get the first to-calculate value of every optimiser
        //and push it onto the todo queue
        {
            auto lck = lock_for();
            for(const auto &child : children)
            {
                bestCalculations[child.get()] = random_calculation();
                push_todo(child->get_next_calculation(previousCalculations[child.get()], &bestCalculations[child.get()], optBoundaries), child.get()); ///@todo later pass previous?
            }
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

    void enable_early_abort(T const& abortVal)
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
    OptCalculation<T> get_best_calculation(OptTarget const& optTarget, T const& targetValue) ///@todo rename to of all, drop params
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

private:
    //targetValue won't be used when maximizing or minimizing
    static bool result_better(OptCalculation<T> const& result, OptCalculation<T> const& other, OptTarget const& optTarget, T const& targetValue) ///@todo consider implementing this in OptCalculation
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

    void add_finished_calculation(IOptAlgorithm<T>* algo, OptCalculation<T> const& optCalculation) ///@todo does too much?
    {
        previousCalculations[algo].push_back(optCalculation);

        {
            auto lck = lock_for();
            finishedCalculations.push_back({optCalculation, this});
        }

        if(result_better(optCalculation, bestCalculations[algo], optTarget, targetValue))
            bestCalculations[algo] = optCalculation;
    }

//------------------------------------------------------------------------------

    unsigned int index_closest_calculation(vector<OptCalculation<T>> const& optCalculations, unsigned int indexThis) const ///@todo could be moved to some helper, also should be easy to simplify heavily
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

    inline optional<lock> lock_for() {
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

            OptCalculation<T> optCalculation = todo.first;
            auto algo = todo.second;

            calcFunction(optCalculation);

            add_finished_calculation(algo, optCalculation);

            if(previousCalculations[algo].size() >= maxCalculations)
                break;

            if(abortEarly)
            {
                OptCalculation<T> calcEarly;
                calcEarly.result = abortValue;

                //if(result_better(optCalculation, calcEarly, pOptBase->optTarget , pOptBase->targetValue))
                //    break;
            }

            //only add the next one if there still are more
            push_todo(algo->get_next_calculation(previousCalculations[algo], &(bestCalculations[algo]), optBoundaries), algo);
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

    OptCalculation<T> random_calculation() const
    {
        OptCalculation<T> optCalculation;
        optCalculation.result = bad_value();
        for(auto const& boundary : optBoundaries)
        {
            T newValue = boundary.second.min + random_factor() * boundary.second.range();
            optCalculation.add_parameter(boundary.second.name, newValue);
        }
        return optCalculation;
    }

    static T random_factor() ///@todo all related functions now defined twice, move to helper
    {
        return rand()/(T)(RAND_MAX);
    }

//------------------------------------------------------------------------------

};

} // namespace cppOpt

#endif // OPTCOORDINATOR_H
