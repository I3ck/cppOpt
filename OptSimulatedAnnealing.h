#ifndef OPTSIMULATEDANNEALING_H
#define OPTSIMULATEDANNEALING_H

#include "OptBase.h"

class OptSimulatedAnnealing : public OptBase
{
private:
    T
        coolingFactor,
        chance;

public:
    OptSimulatedAnnealing(const std::vector<OptBoundary> &optBoundaries,
                          unsigned int maxCalculations,
                          OptTarget optTarget,
                          T coolingFactor,
                          T startChance);
    ~OptSimulatedAnnealing();

    OptValue get_next_value();
};

#endif // OPTSIMULATEDANNEALING_H
