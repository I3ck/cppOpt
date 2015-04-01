#ifndef OPTSIMULATEDANNEALING_H
#define OPTSIMULATEDANNEALING_H

#include "OptBase.h"

namespace cppOpt
{

class OptSimulatedAnnealing : public OptBase
{
private:
    OPT_T
        temperature,
        coolingFactor,
        chance;

public:
    OptSimulatedAnnealing(const OptBoundaries &optBoundaries,
                          unsigned int maxCalculations,
                          SolverBase* pCalculator,
                          OptTarget optTarget,
                          OPT_T targetValue, ///@todo move defaulted ones to the end? or Base => Child like it is currently?
                          OPT_T coolingFactor,
                          OPT_T startChance);
    ~OptSimulatedAnnealing();

private:
    OptCalculation get_next_value();
    OptCalculation random_start_value();
    void update_temperature();
    void update_chance();
};

#endif // OPTSIMULATEDANNEALING_H

} // namespace cppOpt
