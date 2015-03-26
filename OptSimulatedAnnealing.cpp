#include "OptSimulatedAnnealing.h"

OptSimulatedAnnealing::OptSimulatedAnnealing(const std::vector<OptBoundary> &optBoundaries,
                                             unsigned int maxCalculations,
                                             OptTarget optTarget,
                                             T coolingFactor,
                                             T startChance) :
    OptBase(optBoundaries, maxCalculations, optTarget),
    coolingFactor(coolingFactor),
    chance(startChance)
{

}

OptSimulatedAnnealing::~OptSimulatedAnnealing()
{

}

OptValue OptSimulatedAnnealing::get_next_value()
{

}

