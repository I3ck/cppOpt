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

#ifndef OPTTHRESHOLDACCEPTING_H
#define OPTTHRESHOLDACCEPTING_H

#include "OptBase.h"

namespace cppOpt
{

template <typename T>
class OptThresholdAccepting : public OptBase<T>
{
private:

    typedef OptBase<T> super;

    OptCalculation<T>
        optCalculationReference,
        optCalculationConfigurationC;

    const T
        coolingFactor,
        thresholdFactor;

    T
        temperature,
        threshold;

public:
    OptThresholdAccepting(const OptBoundaries<T> &optBoundaries,
                          unsigned int maxCalculations,
                          calc_t<T> calcFunction,
                          OptTarget optTarget,
                          T targetValue,
                          T coolingFactor,
                          T threshold,
                          T thresholdFactor) :
        super(optBoundaries, maxCalculations, move(calcFunction), optTarget, targetValue),
        coolingFactor(coolingFactor),
        thresholdFactor(thresholdFactor),
        temperature(1.0),
        threshold(threshold)
    {

    }

//------------------------------------------------------------------------------

    ~OptThresholdAccepting()
    {

    }

//------------------------------------------------------------------------------

private:
    OptCalculation<T> get_next_calculation()
    {
        if(super::previousCalculations.empty())
            return super::random_start_value();

        OptCalculation<T> newValue;

        if(super::previousCalculations.size() == 1)
        {
            optCalculationReference = super::previousCalculations[0];
            optCalculationConfigurationC = super::previousCalculations[0];

            while(true)
            {
                newValue = OptCalculation<T>();
                for(auto boundary = super::optBoundaries.cbegin(); boundary != super::optBoundaries.cend(); ++boundary)
                {
                    T change = super::calculate_random_change(boundary->second, temperature);

                    newValue.add_parameter(boundary->first, super::previousCalculations[0].get_parameter(boundary->first) + change);
                }
                if(super::valid(newValue))
                    break;
            }

            update_temperature();
            update_threshold();

            return newValue;
        }

        OptCalculation<T> referenceValue;

        if(super::result_better(super::previousCalculations.back(), optCalculationReference, super::optTarget, super::targetValue))
            optCalculationReference = super::previousCalculations.back();

        OptCalculation<T> compareValue = compare_value();

        if(super::result_better(super::previousCalculations.back(), compareValue, super::optTarget, super::targetValue))
            optCalculationConfigurationC = super::previousCalculations.back();

        referenceValue = optCalculationConfigurationC;

        while(true)
        {
            newValue = OptCalculation<T>();
            for(auto boundary = super::optBoundaries.cbegin(); boundary != super::optBoundaries.cend(); ++boundary)
            {
                T change = super::calculate_random_change(boundary->second, temperature);

                newValue.add_parameter(boundary->first, referenceValue.get_parameter(boundary->first) + change);
            }
            if(super::valid(newValue))
                break;
        }

        update_temperature();
        update_threshold();
        return newValue;
    }

//------------------------------------------------------------------------------

    void update_temperature()
    {
        temperature *= coolingFactor;
    }

//------------------------------------------------------------------------------

    void update_threshold()
    {
        threshold *= thresholdFactor;
    }

//------------------------------------------------------------------------------

    OptCalculation<T> compare_value() const
    {
        OptCalculation<T> out;
        switch(super::optTarget)
        {
            case MINIMIZE:
                out.result = optCalculationConfigurationC.result + threshold;
                break;

            case MAXIMIZE:
                out.result = optCalculationConfigurationC.result - threshold;
                break;

            case APPROACH:
                if(super::targetValue > optCalculationConfigurationC.result)
                    out.result = optCalculationConfigurationC.result - threshold;
                else
                    out.result = optCalculationConfigurationC.result + threshold;
                break;

            case DIVERGE:
                if(super::targetValue > optCalculationConfigurationC.result)
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

};

#endif // OPTTHRESHOLDACCEPTING_H

} // namespace cppOpt
