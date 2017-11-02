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

#include "IOptAlgorithm.h"

namespace cppOpt {

template<typename T>
class OptSimulatedAnnealing final : public IOptAlgorithm<T> {
    OptBoundaries<T>
        boundaries;

    const T
        coolingFactor;

    T
        temperature{1.0},
        chance;

    //------------------------------------------------------------------------------

   public:
    OptSimulatedAnnealing(
        OptBoundaries<T> boundaries,
        T                coolingFactor,
        T                startChance) :

                         boundaries(move(boundaries)),
                         coolingFactor(move(coolingFactor)),
                         chance(move(startChance)) {}

    //------------------------------------------------------------------------------

    OptCalculation<T> get_next_calculation(
        vector<OptCalculation<T>> const& previous,
        OptCalculation<T> const*         best) final {
        if (previous.empty() || !best)
            return OptHelper<T>::random_calculation(boundaries);

        OptCalculation<T> referenceValue, newValue;

        if (OptHelper<T>::random_factor() < chance)
            referenceValue = previous.back();
        else
            referenceValue = *best;

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
        update_chance();

        return newValue;
    }

    //------------------------------------------------------------------------------

   private:
    OptBoundaries<T> const& get_boundaries() final {
        return boundaries;
    }

    //------------------------------------------------------------------------------

    void update_temperature() {
        temperature *= coolingFactor;
    }

    //------------------------------------------------------------------------------

    void update_chance() {
        chance *= coolingFactor;
    }
};

#endif  // OPTSIMULATEDANNEALING_H

}  // namespace cppOpt
