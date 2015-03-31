#ifndef OPTSIMULATEDANNEALING_H
#define OPTSIMULATEDANNEALING_H

#include "OptBase.h"

namespace cppOpt
{

class OptSimulatedAnnealing : public OptBase
{
private:
    T
        temperature,
        coolingFactor,
        chance;

public:
    OptSimulatedAnnealing(const OptBoundaries &optBoundaries,
                          unsigned int maxCalculations,
                          SolverBase* pCalculator,
                          OptTarget optTarget,
                          T targetValue, ///@todo move defaulted ones to the end? or Base => Child like it is currently?
                          T coolingFactor,
                          T startChance);
    ~OptSimulatedAnnealing();

private:
    OptValue get_next_value();
    OptValue random_start_value();
    void update_temperature();
    void update_chance();
};

#endif // OPTSIMULATEDANNEALING_H

} // namespace cppOpt
