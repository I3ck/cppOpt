#include "OptSimulatedAnnealing.h"

OptSimulatedAnnealing::OptSimulatedAnnealing(const std::vector<OptBoundary> &optBoundaries,
                                             unsigned int maxCalculations,
                                             CalculatorBase *pCalculator,
                                             OptTarget optTarget,
                                             T coolingFactor,
                                             T startChance) :
    OptBase(optBoundaries, maxCalculations, pCalculator, optTarget),
    temperature(1.0),
    coolingFactor(coolingFactor),
    chance(startChance)
{

}

OptSimulatedAnnealing::~OptSimulatedAnnealing()
{

}

OptValue OptSimulatedAnnealing::get_next_value()
{
    if(previousCalculations.empty())
        return random_start_value();



    update_temperature();
    update_chance();
}

OptValue OptSimulatedAnnealing::random_start_value()
{
    OptValue optValue;
    for(const auto &boundary : optBoundaries)
    {
        T randomFactor = rand()/(T)(RAND_MAX); ///@todo make this a method of OptBase
        T range = boundary.max - boundary.min;
        T newValue = boundary.min + randomFactor * range;
        optValue.add_parameter(boundary.name, newValue);
    }
    return optValue;
}

void OptValue::update_temperature()
{
    temperature *= coolingFactor;
}

void OptValue::update_chance()
{
    chance *= coolingFactor;

}

