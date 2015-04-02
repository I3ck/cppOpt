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

#include "OptEvolutionary.h"

namespace cppOpt
{

OptEvolutionary::OptEvolutionary(const OptBoundaries &optBoundaries,
                                             unsigned int maxCalculations,
                                             OptSolverBase *pCalculator,
                                             OptTarget optTarget,
                                             OPT_T targetValue,
                                             OPT_T coolingFactor,
                                             unsigned int nIndividualsStart,
                                             unsigned int nIndividualsSelection,
                                             unsigned int nIndividualsOffspring,
                                             Opt_T mutation) :
    OptBase(optBoundaries, maxCalculations, pCalculator, optTarget, targetValue),
    coolingFactor(coolingFactor),
    nIndividualsStart(nIndividualsStart),
    nIndividualsSelection(nIndividualsSelection),
    nIndividualsOffspring(nIndividualsOffspring),
    mutation(mutation)
{

}

//------------------------------------------------------------------------------

OptEvolutionary::~OptEvolutionary()
{

}

//------------------------------------------------------------------------------

OptCalculation OptEvolutionary::get_next_calculation()
{
    OptCalculation out;

    if(previousCalculations.empty())
        create_start_individuals();

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

///@todo make this a method of OptBase? (only a "random value" method without defining best)
///@todo also rename from ...value to ...calculation
OptCalculation OptEvolutionary::random_start_value()
{
    OptCalculation optCalculation;
    for(auto boundary = optBoundaries.cbegin(); boundary != optBoundaries.cend(); ++boundary)
    {
        OPT_T range = boundary->max - boundary->min; ///@todo use range method here (and propaply in threshold accepting)
        OPT_T newValue = boundary->min + random_factor() * range;
        optCalculation.add_parameter(boundary->name, newValue);
    }
    bestCalculation = optCalculation;
    bestCalculation.result = bad_value(); ///@todo bestCalculation logic should be moved to general OptBase (since it's gonna repeat itself)
    return optCalculation;
}

//------------------------------------------------------------------------------

void create_start_individuals()
{
    ///@todo
}

//------------------------------------------------------------------------------

void select_individuals()
{
    ///@todo
}

//------------------------------------------------------------------------------

void breed_individuals()
{
    ///@todo
}

//------------------------------------------------------------------------------

void mutate_individuals()
{
    ///@todo
}

//------------------------------------------------------------------------------

void OptEvolutionary::update_mutation()
{
    mutation *= coolingFactor;
}

//------------------------------------------------------------------------------

OPT_T calculate_sort_value(const OptCalculation &optCalculation) const
{
    ///@todo
}

//------------------------------------------------------------------------------

} // namespace cppOpt
