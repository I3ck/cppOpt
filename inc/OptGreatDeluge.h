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
class OptGreatDeluge : public OptBase<T>
{
private:

    typedef OptBase<T> super;

    OptCalculation<T>
        optCalculationReference,
        optCalculationConfigurationC;

    const T
        coolingFactor,
        rain;

    T
        temperature,
        waterLevel;

//------------------------------------------------------------------------------

public:

//------------------------------------------------------------------------------

    OptGreatDeluge(const OptBoundaries<T> &optBoundaries,
                          unsigned int maxCalculations,
                          OptSolverBase<T>* pCalculator,
                          OptTarget optTarget,
                          T targetValue,
                          T coolingFactor,
                          T waterLevel,
                          T rain) :
        super(optBoundaries, maxCalculations, pCalculator, optTarget, targetValue),
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

    OptCalculation<T> get_next_calculation()
    {
        if(super::previousCalculations.empty())
            return super::random_start_value();

        OptCalculation<T>
            newValue,
            referenceValue,
            compareValue = compare_value();

        if(super::result_better(super::previousCalculations.back(), compareValue, super::optTarget, super::targetValue))
            referenceValue = super::previousCalculations.back();

        else
            referenceValue = super::bestCalculation;

        while(true)
        {
            newValue = OptCalculation<T>();
            for(auto boundary = super::optBoundaries.cbegin(); boundary != super::optBoundaries.cend(); ++boundary)
            {
                T change = super::calculate_random_change(*boundary, temperature);

                newValue.add_parameter(boundary->name, referenceValue.get_parameter(boundary->name) + change);
            }
            if(super::valid(newValue))
                break;
        }

        update_temperature();
        update_water_level();
        return newValue;
    }

//------------------------------------------------------------------------------

    void update_temperature()
    {
        temperature *= coolingFactor;
    }

//------------------------------------------------------------------------------

    void update_water_level()
    {
        switch(super::optTarget)
        {
            case MINIMIZE:
                waterLevel -= rain;
                break;

            case MAXIMIZE:
                waterLevel += rain;
                break;

            case APPROACH:
                if(super::targetValue > waterLevel)
                    waterLevel += rain;
                else
                    waterLevel -= rain;
                break;

            case DIVERGE:
                if(super::targetValue > waterLevel)
                    waterLevel -= rain;
                else
                    waterLevel += rain;
                break;

            default: // MINIMIZE
                waterLevel -= rain;
        }
    }

//------------------------------------------------------------------------------

    OptCalculation<T> compare_value() const
    {
        OptCalculation<T> out;
        out.result = waterLevel;
        return out;
    }

//------------------------------------------------------------------------------

};

#endif // OPTGREATDELUGE_H

} // namespace cppOpt
