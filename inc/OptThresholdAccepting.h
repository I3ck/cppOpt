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

class OptThresholdAccepting : public OptBase
{
private:
    OptCalculation
        optCalculationReference,
        optCalculationConfigurationC;

    OPT_T
        temperature,
        coolingFactor,
        threshold,
        thresholdFactor;

public:
    OptThresholdAccepting(const OptBoundaries &optBoundaries,
                          unsigned int maxCalculations,
                          OptSolverBase* pCalculator,
                          OptTarget optTarget,
                          OPT_T targetValue, ///@todo move defaulted ones to the end? or Base => Child like it is currently?
                          OPT_T coolingFactor,
                          OPT_T threshold,
                          OPT_T thresholdFactor);

    ~OptThresholdAccepting();

private:
    OptCalculation get_next_calculation();
    OptCalculation random_start_value();
    void update_temperature();
    void update_threshold();
    OptCalculation compare_value() const; ///@todo rename & use T as return value? (=> would require a new compare method)
};

#endif // OPTTHRESHOLDACCEPTING_H

} // namespace cppOpt
