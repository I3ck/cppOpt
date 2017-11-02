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

#include "IOptAlgorithm.h"

namespace cppOpt {

template<typename T>
class OptThresholdAccepting final : public IOptAlgorithm<T> {
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
        thresholdFactor;

    T
        temperature{1.0},
        threshold;

   public:
    OptThresholdAccepting(
        OptBoundaries<T> boundaries,
        OptTarget        optTarget,
        T                targetValue,
        T                coolingFactor,
        T                threshold,
        T                thresholdFactor) :

                             boundaries(move(boundaries)),
                             optTarget(move(optTarget)),
                             targetValue(move(targetValue)),
                             coolingFactor(move(coolingFactor)),
                             thresholdFactor(move(thresholdFactor)),
                             threshold(move(threshold)) {}

    //------------------------------------------------------------------------------

    OptCalculation<T> get_next_calculation(
        vector<OptCalculation<T>> const& previous,
        OptCalculation<T> const*         best) final {
        if (previous.empty() || !best)
            return OptHelper<T>::random_calculation(boundaries);

        OptCalculation<T> newValue;

        if (previous.size() == 1) {
            optCalculationReference      = previous[0];
            optCalculationConfigurationC = previous[0];

            while (true) {
                newValue = OptCalculation<T>();
                for (auto const& boundary : boundaries) {
                    T change = OptHelper<T>::calculate_random_change(boundary.second, temperature);

                    newValue.add_parameter(boundary.first, previous[0].get_parameter(boundary.first) + change);
                }
                if (OptHelper<T>::valid(newValue, boundaries))
                    break;
            }

            update_temperature();
            update_threshold();

            return newValue;
        }

        OptCalculation<T> referenceValue;

        if (OptHelper<T>::result_better(previous.back(), optCalculationReference, optTarget, targetValue))
            optCalculationReference = previous.back();

        OptCalculation<T> compareValue = compare_value();

        if (OptHelper<T>::result_better(previous.back(), compareValue, optTarget, targetValue))
            optCalculationConfigurationC = previous.back();

        referenceValue = optCalculationConfigurationC;

        while (true) {
            newValue = OptCalculation<T>();
            for (auto const& boundary : boundaries) {
                T change = OptHelper<T>::calculate_random_change(boundary.second, temperature);

                newValue.add_parameter(boundary.first, referenceValue.get_parameter(boundary.first) + change);
            }
            if (OptHelper<T>::valid(newValue, boundaries))
                break;
        }

        update_temperature();
        update_threshold();
        return newValue;
    }

    //------------------------------------------------------------------------------

    OptBoundaries<T> const& get_boundaries() final {
        return boundaries;
    }

    //------------------------------------------------------------------------------

   private:
    void update_temperature() {
        temperature *= coolingFactor;
    }

    //------------------------------------------------------------------------------

    void update_threshold() {
        threshold *= thresholdFactor;
    }

    //------------------------------------------------------------------------------

    OptCalculation<T> compare_value() const {
        OptCalculation<T> out;
        switch (optTarget) {
            case OptTarget::MINIMIZE:
                out.result = optCalculationConfigurationC.result + threshold;
                break;

            case OptTarget::MAXIMIZE:
                out.result = optCalculationConfigurationC.result - threshold;
                break;

            case OptTarget::APPROACH:
                if (targetValue > optCalculationConfigurationC.result)
                    out.result = optCalculationConfigurationC.result - threshold;
                else
                    out.result = optCalculationConfigurationC.result + threshold;
                break;

            case OptTarget::DIVERGE:
                if (targetValue > optCalculationConfigurationC.result)
                    out.result = optCalculationConfigurationC.result + threshold;
                else
                    out.result = optCalculationConfigurationC.result - threshold;
                break;

            default:  // MINIMIZE
                out.result = optCalculationConfigurationC.result + threshold;
        }
        return out;
    }

    //------------------------------------------------------------------------------
};

#endif  // OPTTHRESHOLDACCEPTING_H

}  // namespace cppOpt
