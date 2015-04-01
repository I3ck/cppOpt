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

public:
    OptGreatDeluge(const OptBoundaries &optBoundaries,
                          unsigned int maxCalculations,
                          OptSolverBase* pCalculator,
                          OptTarget optTarget,
                          OPT_T targetValue, ///@todo move defaulted ones to the end? or Base => Child like it is currently?
                          OPT_T coolingFactor,
                          OPT_T waterLevel,
                          OPT_T rain);

    ~OptGreatDeluge();

private:
    OptCalculation get_next_calculation();
    OptCalculation random_start_value();
    void update_temperature();
    void update_water_level();

    OptCalculation compare_value() const; ///@todo rename & use T as return value? (=> would require a new compare method)
};

#endif // OPTGREATDELUGE_H

} // namespace cppOpt
