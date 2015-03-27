#ifndef OPTSIMULATEDANNEALING_H
#define OPTSIMULATEDANNEALING_H

#include "OptBase.h"

class OptSimulatedAnnealing : public OptBase
{
private:
    T
        temperature,
        coolingFactor,
        chance;

public:
    OptSimulatedAnnealing(const std::vector<OptBoundary> &optBoundaries,
                          unsigned int maxCalculations,
                          CalculatorBase* pCalculator,
                          OptTarget optTarget,
                          T coolingFactor,
                          T startChance);
    ~OptSimulatedAnnealing();

    OptValue get_next_value();
    OptValue random_start_value();
};

#endif // OPTSIMULATEDANNEALING_H
