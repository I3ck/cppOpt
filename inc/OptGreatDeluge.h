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

#include "IOptAlgorithm.h"

namespace cppOpt
{

template <typename T>
class OptGreatDeluge final : public IOptAlgorithm<T>
{
    OptBoundaries<T>
        boundaries;

    const OptTarget
        optTarget;

    const T
        targetValue;

    OptCalculation<T>
        optCalculationReference,
        optCalculationConfigurationC;

    const T
        coolingFactor,
        rain;

    T
        temperature{1.0},
        waterLevel;

public:

//------------------------------------------------------------------------------

    OptGreatDeluge(
        OptBoundaries<T> boundaries,
        OptTarget optTarget,
        T targetValue,
        T coolingFactor,
        T waterLevel,
        T rain) :

        boundaries(move(boundaries)),
        optTarget(move(optTarget)),
        targetValue(move(targetValue)),
        coolingFactor(move(coolingFactor)),
        rain(move(rain)),
        waterLevel(move(waterLevel))
    {}

//------------------------------------------------------------------------------

    OptCalculation<T> get_next_calculation(
        vector<OptCalculation<T>> const& previous,
        OptCalculation<T>         const* best) final
    {
        if(previous.empty() || !best)
            return OptHelper<T>::random_calculation(boundaries);

        OptCalculation<T>
            newValue,
            referenceValue,
            compareValue = compare_value();

        if(OptHelper<T>::result_better(previous.back(), compareValue, optTarget, targetValue))
            referenceValue = previous.back();

        else
            referenceValue = *best;

        while(true)
        {
            newValue = OptCalculation<T>();
            for(auto const& boundary : boundaries)
            {
                T change = OptHelper<T>::calculate_random_change(boundary.second, temperature);

                newValue.add_parameter(boundary.first, referenceValue.get_parameter(boundary.first) + change);
            }
            if(OptHelper<T>::valid(newValue, boundaries))
                break;
        }

        update_temperature();
        update_water_level();
        return newValue;
    }

//------------------------------------------------------------------------------

    OptBoundaries<T> const& get_boundaries() final
    {
        return boundaries;
    }

//------------------------------------------------------------------------------

private:

    void update_temperature()
    {
        temperature *= coolingFactor;
    }

//------------------------------------------------------------------------------

    void update_water_level()
    {
        switch(optTarget)
        {
            case OptTarget::MINIMIZE:
                waterLevel -= rain;
                break;

            case OptTarget::MAXIMIZE:
                waterLevel += rain;
                break;

            case OptTarget::APPROACH:
                if(targetValue > waterLevel)
                    waterLevel += rain;
                else
                    waterLevel -= rain;
                break;

            case OptTarget::DIVERGE:
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
