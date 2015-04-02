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

#ifndef OPTSIMULATEDANNEALING_H
#define OPTSIMULATEDANNEALING_H

#include "OptBase.h"

namespace cppOpt
{

class OptEvolutionary : public OptBase
{
private:

    const unsigned int
        nIndividualsStart,
        nIndividualsSelection,
        nIndividualsOffspring;

    const OPT_T
        coolingFactor;

    OPT_T
        mutation,
        chance;

    std::multimap<OPT_T, OptCalculation> // <SORT_VALUE, CALCULATION>
        previousCalculationsSorted;

    std::vector<OptCalculation> ///@todo might be better to use lists here
        individualsStart,
        individualsSelected,
        individualsBred,
        individualsMutated; ///@todo might be better to use a stack/queue here

public:
    OptEvolutionary(const OptBoundaries &optBoundaries,
                          unsigned int maxCalculations,
                          OptSolverBase* pCalculator,
                          OptTarget optTarget,
                          OPT_T targetValue, ///@todo move defaulted ones to the end? or Base => Child like it is currently?
                          OPT_T coolingFactor,
                          unsigned int nIndividualsStart,
                          unsigned int nIndividualsSelection,
                          unsigned int nIndividualsOffspring,
                          OPT_T mutation);
    ~OptEvolutionary();

private:
    OptCalculation get_next_calculation();
    OptCalculation random_start_value();
    void add_previous_to_sorted();
    void create_start_individuals();
    void select_individuals();
    void breed_individuals();
    void mutate_individuals();

    void update_mutation();

    OPT_T calculate_sort_value(const OptCalculation &optCalculation) const;
};

#endif // OPTSIMULATEDANNEALING_H

} // namespace cppOpt
