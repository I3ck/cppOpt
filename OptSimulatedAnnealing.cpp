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

    OptValue referenceValue, newValue;

    if(random_factor() < chance)
        referenceValue = previousCalculations.back();

    else
        referenceValue = bestCalculation; ///@todo rename bestCalculation to bestOptValue or similar

    for(const auto &boundary : optBoundaries)
    {
        ///@todo change logic could be a method
        T change, range, maxChange;

        range = boundary.max - boundary.min; ///@todo maybe add range method to boundary
        maxChange = 0.5 * range * temperature;
        change = random_factor() * maxChange;

        if(rand() % 2)
            change *= -1.0;
#ifdef DEBUG
        std::cout << "DEBUG: in simulated annealing" << std::endl;
        std::cout << "DEBUG: name is " << boundary.name << std::endl;
#endif
        newValue.add_parameter(boundary.name, referenceValue.get_parameter(boundary.name) + change);
    }

    update_temperature();
    update_chance();

    return newValue;
}

OptValue OptSimulatedAnnealing::random_start_value()
{
    OptValue optValue;
    for(const auto &boundary : optBoundaries)
    {
        T range = boundary.max - boundary.min;
        T newValue = boundary.min + random_factor() * range;
        optValue.add_parameter(boundary.name, newValue);
    }
    bestCalculation = optValue;
    bestCalculation.result = bad_value(); ///@todo bestCalculation logic should be moved to general OptBase (since it's gonna repeat itself)
    return optValue;
}

void OptSimulatedAnnealing::update_temperature()
{
    temperature *= coolingFactor;
}

void OptSimulatedAnnealing::update_chance()
{
    chance *= coolingFactor;
}

