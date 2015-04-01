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

#include "OptGreatDeluge.h"

namespace cppOpt
{

OptGreatDeluge::OptGreatDeluge(const OptBoundaries &optBoundaries,
                                             unsigned int maxCalculations,
                                             OptSolverBase* pCalculator,
                                             OptTarget optTarget,
                                             OPT_T targetValue,
                                             OPT_T coolingFactor,
                                             OPT_T waterLevel,
                                             OPT_T rain) :
    OptBase(optBoundaries, maxCalculations, pCalculator, optTarget, targetValue),
    temperature(1.0),
    coolingFactor(coolingFactor),
    waterLevel(waterLevel),
    rain(rain)
{

}

//------------------------------------------------------------------------------

OptGreatDeluge::~OptGreatDeluge()
{

}

//------------------------------------------------------------------------------

OptCalculation OptGreatDeluge::get_next_value()
{
    if(previousCalculations.empty())
        return random_start_value();

    ///@todo merge these definitions
    OptCalculation newValue;
    OptCalculation center;

    OptCalculation compareValue = compare_value();

    if(result_better(previousCalculations.back(), compareValue, optTarget, targetValue))
        center = previousCalculations.back();

    else
        center = bestCalculation;

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
    update_water_level();
    return newValue;
}

//------------------------------------------------------------------------------

OptCalculation OptGreatDeluge::random_start_value()
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

void OptGreatDeluge::update_temperature()
{
    temperature *= coolingFactor;
}

//------------------------------------------------------------------------------

void OptGreatDeluge::update_water_level()
{
    switch(optTarget)
    {
        case MINIMIZE:
            waterLevel -= rain;
            break;

        case MAXIMIZE:
            waterLevel += rain;
            break;

        case APPROACH:
            if(targetValue > waterLevel)
                waterLevel += rain;
            else
                waterLevel -= rain;
            break;

        case DIVERGE:
            if(targetValue > waterLevel)
                waterLevel -= rain;
            else
                waterLevel += rain;
            break;

        default: // MINIMIZE
            waterLevel -= rain;
    }
}

//------------------------------------------------------------------------------

OptCalculation OptGreatDeluge::compare_value() const
{
    OptCalculation out;
    out.result = waterLevel;
    return out;
}

//------------------------------------------------------------------------------

} // namespace cppOpt
