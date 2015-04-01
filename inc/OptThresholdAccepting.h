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
                          SolverBase* pCalculator,
                          OptTarget optTarget,
                          OPT_T targetValue, ///@todo move defaulted ones to the end? or Base => Child like it is currently?
                          OPT_T coolingFactor,
                          OPT_T threshold,
                          OPT_T thresholdFactor);

    ~OptThresholdAccepting();

private:
    OptCalculation get_next_value();
    OptCalculation random_start_value();
    void update_temperature();
    void update_threshold();
    OptCalculation compare_value() const; ///@todo rename & use T as return value? (=> would require a new compare method)
};

#endif // OPTTHRESHOLDACCEPTING_H

} // namespace cppOpt
