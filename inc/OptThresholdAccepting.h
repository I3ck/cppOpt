#ifndef OPTTHRESHOLDACCEPTING_H
#define OPTTHRESHOLDACCEPTING_H

#include "OptBase.h"

namespace cppOpt
{

class OptThresholdAccepting : public OptBase
{
private:
    OptValue
        optValueReference,
        optValueConfigurationC;

    T
        temperature,
        coolingFactor,
        threshold,
        thresholdFactor;

public:
    OptThresholdAccepting(const OptBoundaries &optBoundaries,
                          unsigned int maxCalculations,
                          SolverBase* pCalculator,
                          OptTarget optTarget,
                          T targetValue, ///@todo move defaulted ones to the end? or Base => Child like it is currently?
                          T coolingFactor,
                          T threshold,
                          T thresholdFactor);

    ~OptThresholdAccepting();

private:
    OptValue get_next_value();
    OptValue random_start_value();
    void update_temperature();
    void update_threshold();
    OptValue compare_value() const; ///@todo rename & use T as return value? (=> would require a new compare method)
};

#endif // OPTTHRESHOLDACCEPTING_H

} // namespace cppOpt
