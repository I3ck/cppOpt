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

#ifndef OPTEVOLUTIONARY_H
#define OPTEVOLUTIONARY_H

#include "IOptAlgorithm.h"

namespace cppOpt
{

template <typename T>
class OptEvolutionary final : public IOptAlgorithm<T>
{
    OptBoundaries<T>
        boundaries;

    const OptTarget
        optTarget;

    const T
        targetValue;

    const T
        coolingFactor;

    const unsigned int
        nIndividualsStart,
        nIndividualsSelection,
        nIndividualsOffspring;

    T
        mutation,
        chance;


    multimap< T, OptCalculation<T> > // <SORT_VALUE, CALCULATION>
        previousCalculationsSorted;

    vector< OptCalculation<T> >
        individualsStart,
        individualsSelected;

    queue< OptCalculation<T> >
        individualsBred,
        individualsMutated;

//------------------------------------------------------------------------------

public:
    OptEvolutionary(
        OptBoundaries<T> boundaries,
        OptTarget optTarget,
        T targetValue,
        T coolingFactor,
        unsigned int nIndividualsStart,
        unsigned int nIndividualsSelection,
        unsigned int nIndividualsOffspring,
        T mutation) :

        boundaries(move(boundaries)),
        optTarget(move(optTarget)),
        targetValue(move(targetValue)),
        coolingFactor(move(coolingFactor)),
        nIndividualsStart(nIndividualsStart),
        nIndividualsSelection(nIndividualsSelection),
        nIndividualsOffspring(nIndividualsOffspring),
        mutation(move(mutation))
    {}

//------------------------------------------------------------------------------

    OptCalculation<T> get_next_calculation(
        vector<OptCalculation<T>> const& previous,
        OptCalculation<T>         const* best) final
    {
        if(previous.empty() || !best)
            create_start_individuals();
        else
            add_previous_to_sorted(previous);

        OptCalculation<T> out;

        if(individualsStart.size() != 0)
        {
            out = individualsStart.back();
            individualsStart.pop_back();

            update_mutation();

            return out;
        }

        if(individualsMutated.size() == 0)
        {
            select_individuals();
            breed_individuals();
            mutate_individuals();
        }

        out = individualsMutated.front();
        individualsMutated.pop();

        update_mutation();

        return out;
    }

//------------------------------------------------------------------------------

    OptBoundaries<T> const& get_boundaries() final
    {
        return boundaries;
    }

//------------------------------------------------------------------------------

private:

    void add_previous_to_sorted(vector<OptCalculation<T>> const& previous)
    {
        if(previous.size() > 0)
        {
            T sortValue = calculate_sort_value(previous.back());
            previousCalculationsSorted.emplace(sortValue, previous.back());
        }
    }

//------------------------------------------------------------------------------

    void create_start_individuals()
    {
        individualsStart.push_back(OptHelper<T>::random_calculation(boundaries));

        for(unsigned int i = 1; i < nIndividualsStart; ++i)
        {
            OptCalculation<T> optCalculation = OptHelper<T>::random_calculation(boundaries);
            individualsStart.push_back(optCalculation);
        }
    }

//------------------------------------------------------------------------------

    void select_individuals()
    {
        unsigned int i = 1;

        for(const auto &individual : previousCalculationsSorted)
        {
            individualsSelected.push_back(individual.second);

            if(i >= nIndividualsSelection)
                break;

            ++i;
        }
    }

//------------------------------------------------------------------------------

    void breed_individuals()
    {
        set<unsigned int> usedIndexes;
        vector< pair< OptCalculation<T> ,OptCalculation<T> > > parents;

        for(unsigned int i = 0; i < individualsSelected.size(); ++i)
        {
            unsigned int indexClosest = index_closest_calculation(individualsSelected, i);

            if(usedIndexes.find(indexClosest) == usedIndexes.end())
            {
                parents.emplace_back(individualsSelected[i], individualsSelected[indexClosest]);
                usedIndexes.insert(indexClosest);
            }
        }

        individualsSelected.clear();
        for(const auto &parentPair : parents)
        {
            for(unsigned int i = 0; i < nIndividualsOffspring; ++i)
                individualsBred.push(  parentPair.first.calculation_between(parentPair.second)  );
        }
    }

//------------------------------------------------------------------------------

    void mutate_individuals()
    {
        while(individualsBred.size() != 0)
        {
            auto individual = individualsBred.front();
            individualsBred.pop();

            OptCalculation<T> mutatedIndividual;
            for(auto const& boundary : boundaries)
            {
                T change, maxChange;

                maxChange = 0.5 * boundary.second.range() * mutation;
                change = OptHelper<T>::random_factor() * maxChange;

                if(rand() % 2)
                    change *= -1.0;

                mutatedIndividual.add_parameter(boundary.second.name, individual.get_parameter(boundary.second.name) + change);

            }
            individualsMutated.push(mutatedIndividual);
        }
    }

//------------------------------------------------------------------------------

    void update_mutation()
    {
        mutation *= coolingFactor;
    }

//------------------------------------------------------------------------------

    T calculate_sort_value(const OptCalculation<T> &optCalculation) const
    {
        switch(optTarget)
        {
            case OptTarget::MINIMIZE:
                return optCalculation.result;

            case OptTarget::MAXIMIZE:
                return -optCalculation.result;

            case OptTarget::APPROACH:
                return fabs(targetValue - optCalculation.result);

            case OptTarget::DIVERGE:
                return -fabs(targetValue - optCalculation.result);

            default: // MINIMIZE
                return optCalculation.result;
        }
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

};

#endif // OPTEVOLUTIONARY_H

} // namespace cppOpt
