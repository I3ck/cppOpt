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

#include "OptBase.h"

namespace cppOpt
{

template <typename T>
class OptEvolutionary : public OptBase<T>
{
private:

    typedef OptBase<T> super;

    const unsigned int
        nIndividualsStart,
        nIndividualsSelection,
        nIndividualsOffspring;

    const T
        coolingFactor;

    T
        mutation,
        chance;

    std::multimap< T, OptCalculation<T> > // <SORT_VALUE, CALCULATION>
        previousCalculationsSorted;

    std::vector< OptCalculation<T> >
        individualsStart,
        individualsSelected,
        individualsBred,
        individualsMutated; ///@todo might be better to use a stack/queue here

//------------------------------------------------------------------------------

public:
    OptEvolutionary(const OptBoundaries<T> &optBoundaries,
                          unsigned int maxCalculations,
                          OptSolverBase<T>* pCalculator,
                          OptTarget optTarget,
                          T targetValue,
                          T coolingFactor,
                          unsigned int nIndividualsStart,
                          unsigned int nIndividualsSelection,
                          unsigned int nIndividualsOffspring,
                          T mutation) :
        super(optBoundaries, maxCalculations, pCalculator, optTarget, targetValue),
        coolingFactor(coolingFactor),
        nIndividualsStart(nIndividualsStart),
        nIndividualsSelection(nIndividualsSelection),
        nIndividualsOffspring(nIndividualsOffspring),
        mutation(mutation)
    {
        individualsStart.reserve(nIndividualsStart);
        individualsSelected.reserve(nIndividualsSelection);
        individualsBred.reserve(  ceil(nIndividualsOffspring * nIndividualsSelection / 2.0)  );
        individualsMutated.reserve(  ceil(nIndividualsOffspring * nIndividualsSelection / 2.0)  );

    }

//------------------------------------------------------------------------------

    ~OptEvolutionary()
    {

    }

//------------------------------------------------------------------------------

private:

//------------------------------------------------------------------------------

    OptCalculation<T> get_next_calculation()
    {
        OptCalculation<T> out;

        if(super::previousCalculations.empty())
            create_start_individuals();
        else
            add_previous_to_sorted();

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

        out = individualsMutated.back();
        individualsMutated.pop_back();

        update_mutation();

        return out;
    }

//------------------------------------------------------------------------------

    OptCalculation<T> random_start_value()
    {
        OptCalculation<T> optCalculation;
        for(auto boundary = super::optBoundaries.cbegin(); boundary != super::optBoundaries.cend(); ++boundary)
        {
            T newValue = boundary->min + super::random_factor() * boundary->range();
            optCalculation.add_parameter(boundary->name, newValue);
        }
        super::bestCalculation = optCalculation;
        super::bestCalculation.result = super::bad_value(); ///@todo bestCalculation logic should be moved to general OptBase (since it's gonna repeat itself)
        return optCalculation;
    }

//------------------------------------------------------------------------------

    void add_previous_to_sorted()
    {
        if(super::previousCalculations.size() > 0)
        {
            T sortValue = calculate_sort_value(super::previousCalculations.back());
            previousCalculationsSorted.emplace(sortValue, super::previousCalculations.back());
        }
    }

//------------------------------------------------------------------------------

    void create_start_individuals()
    {
        individualsStart.push_back(random_start_value());

        for(unsigned int i = 1; i < nIndividualsStart; ++i)
        {
            OptCalculation<T> optCalculation = super::random_calculation();
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
        std::set<unsigned int> usedIndexes;
        std::vector< std::pair< OptCalculation<T> ,OptCalculation<T> > > parents;

        ///@todo needs heavy testing
        ///@todo make this its own method? (and parents a member?)
        for(unsigned int i = 0; i < individualsSelected.size(); ++i)
        {
            unsigned int indexClosest = index_closest(individualsSelected, i);

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
                individualsBred.push_back(  parentPair.first.calculation_between(parentPair.second)  );
        }
    }

//------------------------------------------------------------------------------

    void mutate_individuals()
    {
        for(auto individual : individualsBred)
        {
            OptCalculation<T> mutatedIndividual;
            for(auto boundary = super::optBoundaries.cbegin(); boundary != super::optBoundaries.cend(); ++boundary)
            {
                T change, maxChange;

                maxChange = 0.5 * boundary->range() * mutation;
                change = super::random_factor() * maxChange;

                if(rand() % 2)
                    change *= -1.0;

                mutatedIndividual.add_parameter(boundary->name, individual.get_parameter(boundary->name) + change);

            }
            individualsMutated.push_back(mutatedIndividual);
        }
        individualsBred.clear();
    }

//------------------------------------------------------------------------------

    void update_mutation()
    {
        mutation *= coolingFactor;
    }

//------------------------------------------------------------------------------

    T calculate_sort_value(const OptCalculation<T> &optCalculation) const
    {
        switch(super::optTarget)
        {
            case MINIMIZE:
                return optCalculation.result;

            case MAXIMIZE:
                return -optCalculation.result;

            case APPROACH:
                return fabs(super::targetValue - optCalculation.result);

            case DIVERGE:
                return -fabs(super::targetValue - optCalculation.result);

            default: // MINIMIZE
                return optCalculation.result;
        }
    }

//------------------------------------------------------------------------------

    ///@todo could be static or even a member of OptBase
    unsigned int index_closest(const std::vector< OptCalculation<T> > &optCalculations, unsigned int indexThis) const
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
