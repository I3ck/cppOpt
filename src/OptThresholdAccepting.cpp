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

#include "OptThresholdAccepting.h"

namespace cppOpt
{

OptThresholdAccepting::OptThresholdAccepting(const OptBoundaries &optBoundaries,
                                             unsigned int maxCalculations,
                                             OptSolverBase* pCalculator,
                                             OptTarget optTarget,
                                             OPT_T targetValue,
                                             OPT_T coolingFactor,
                                             OPT_T threshold,
                                             OPT_T thresholdFactor) :
    OptBase(optBoundaries, maxCalculations, pCalculator, optTarget, targetValue),
    temperature(1.0),
    coolingFactor(coolingFactor),
    threshold(threshold),
    thresholdFactor(thresholdFactor)
{

}

//------------------------------------------------------------------------------

OptThresholdAccepting::~OptThresholdAccepting()
{

}

//------------------------------------------------------------------------------

OptCalculation OptThresholdAccepting::get_next_value()
{
    if(previousCalculations.empty())
        return random_start_value();

    OptCalculation newValue;

    if(previousCalculations.size() == 1)
    {
        optCalculationReference = previousCalculations[0];
        optCalculationConfigurationC = previousCalculations[0];

        while(true)
        {
            newValue = OptCalculation();
            for(auto boundary = optBoundaries.cbegin(); boundary != optBoundaries.cend(); ++boundary)
            {
                ///@todo change logic could be a method
                OPT_T change, maxChange;

                maxChange = 0.5 * boundary->range() * temperature;
                change = random_factor() * maxChange;

                if(rand() % 2)
                    change *= -1.0;

                newValue.add_parameter(boundary->name, previousCalculations[0].get_parameter(boundary->name) + change);
            }
            if(valid(newValue))
                break;
        }

        update_temperature();
        update_threshold();

        return newValue;
    }

    OptCalculation center;

    if(result_better(previousCalculations.back(), optCalculationReference, optTarget, targetValue))
        optCalculationReference = previousCalculations.back();

    OptCalculation compareValue = compare_value();

    if(result_better(previousCalculations.back(), compareValue, optTarget, targetValue))
        optCalculationConfigurationC = previousCalculations.back();

    center = optCalculationConfigurationC;

    while(true)
    {
        newValue = OptCalculation();
        for(auto boundary = optBoundaries.cbegin(); boundary != optBoundaries.cend(); ++boundary)
        {
            ///@todo change logic could be a method
            OPT_T change, maxChange;

            maxChange = 0.5 * boundary->range() * temperature;
            change = random_factor() * maxChange;

            if(rand() % 2)
                change *= -1.0;

            newValue.add_parameter(boundary->name, center.get_parameter(boundary->name) + change);
        }
        if(valid(newValue))
            break;
    }

    update_temperature();
    update_threshold();
    return newValue;
}

//------------------------------------------------------------------------------

OptCalculation OptThresholdAccepting::random_start_value()
{
    OptCalculation optCalculation;
    for(auto boundary = optBoundaries.cbegin(); boundary != optBoundaries.cend(); ++boundary)
    {
        OPT_T range = boundary->max - boundary->min;
        OPT_T newValue = boundary->min + random_factor() * range;
        optCalculation.add_parameter(boundary->name, newValue);
    }
    bestCalculation = optCalculation;
    bestCalculation.result = bad_value(); ///@todo bestCalculation logic should be moved to general OptBase (since it's gonna repeat itself)
    return optCalculation;
}

//------------------------------------------------------------------------------

void OptThresholdAccepting::update_temperature()
{
    temperature *= coolingFactor;
}

//------------------------------------------------------------------------------

void OptThresholdAccepting::update_threshold()
{
    threshold *= thresholdFactor;
}

//------------------------------------------------------------------------------

OptCalculation OptThresholdAccepting::compare_value() const
{
    OptCalculation out;
    switch(optTarget)
    {
        case MINIMIZE:
            out.result = optCalculationConfigurationC.result + threshold;
            break;

        case MAXIMIZE:
            out.result = optCalculationConfigurationC.result - threshold;
            break;

        case APPROACH:
            if(targetValue > optCalculationConfigurationC.result)
                out.result = optCalculationConfigurationC.result - threshold;
            else
                out.result = optCalculationConfigurationC.result + threshold;
            break;

        case DIVERGE:
            if(targetValue > optCalculationConfigurationC.result)
                out.result = optCalculationConfigurationC.result + threshold;
            else
                out.result = optCalculationConfigurationC.result - threshold;
            break;

        default: // MINIMIZE
            out.result = optCalculationConfigurationC.result + threshold;
    }
    return out;
}

//------------------------------------------------------------------------------

} // namespace cppOpt
