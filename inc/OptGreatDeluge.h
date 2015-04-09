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

#ifndef OPTGREATDELUGE_H
#define OPTGREATDELUGE_H

#include "OptBase.h"

namespace cppOpt
{

template <typename T>
class OptGreatDeluge : public OptBase
{
private:
    OptCalculation
        optCalculationReference,
        optCalculationConfigurationC;

    const OPT_T
        coolingFactor,
        rain;

    OPT_T
        temperature,
        waterLevel;

//------------------------------------------------------------------------------

public:

//------------------------------------------------------------------------------

    OptGreatDeluge(const OptBoundaries &optBoundaries,
                          unsigned int maxCalculations,
                          OptSolverBase* pCalculator,
                          OptTarget optTarget,
                          OPT_T targetValue, ///@todo move defaulted ones to the end? or Base => Child like it is currently?
                          OPT_T coolingFactor,
                          OPT_T waterLevel,
                          OPT_T rain) : 
        OptBase(optBoundaries, maxCalculations, pCalculator, optTarget, targetValue),
        coolingFactor(coolingFactor),
        rain(rain),
        temperature(1.0),
        waterLevel(waterLevel)
    {

    }

//------------------------------------------------------------------------------

    ~OptGreatDeluge()
    {

    }

//------------------------------------------------------------------------------

private:

//------------------------------------------------------------------------------

    OptCalculation get_next_calculation()
    {
        if(previousCalculations.empty())
            return random_start_value();

        ///@todo merge these definitions
        OptCalculation newValue;
        OptCalculation referenceValue;

        OptCalculation compareValue = compare_value();

        if(result_better(previousCalculations.back(), compareValue, optTarget, targetValue))
            referenceValue = previousCalculations.back();

        else
            referenceValue = bestCalculation;

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

                newValue.add_parameter(boundary->name, referenceValue.get_parameter(boundary->name) + change);
            }
            if(valid(newValue))
                break;
        }

        update_temperature();
        update_water_level();
        return newValue;
    }

//------------------------------------------------------------------------------

    OptCalculation random_start_value()
    {
        OptCalculation optCalculation = random_calculation();
        bestCalculation = optCalculation;
        bestCalculation.result = bad_value(); ///@todo bestCalculation logic should be moved to general OptBase (since it's gonna repeat itself)
        return optCalculation;
    }

//------------------------------------------------------------------------------

    void update_temperature()
    {
        temperature *= coolingFactor;
    }

//------------------------------------------------------------------------------

    void update_water_level()
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

    OptCalculation compare_value() const ///@todo rename & use T as return value? (=> would require a new compare method)
    {
        OptCalculation out;
        out.result = waterLevel;
        return out;
    }

//------------------------------------------------------------------------------

};

#endif // OPTGREATDELUGE_H

} // namespace cppOpt
