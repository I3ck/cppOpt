#include "OptSimulatedAnnealing.h"

namespace cppOpt
{

OptSimulatedAnnealing::OptSimulatedAnnealing(const OptBoundaries &optBoundaries,
                                             unsigned int maxCalculations,
                                             SolverBase *pCalculator,
                                             OptTarget optTarget,
                                             T targetValue,
                                             T coolingFactor,
                                             T startChance) :
    OptBase(optBoundaries, maxCalculations, pCalculator, optTarget, targetValue),
    temperature(1.0),
    coolingFactor(coolingFactor),
    chance(startChance)
{

}

//------------------------------------------------------------------------------

OptSimulatedAnnealing::~OptSimulatedAnnealing()
{

}

//------------------------------------------------------------------------------

OptValue OptSimulatedAnnealing::get_next_value()
{
    if(previousCalculations.empty())
        return random_start_value();

    OptValue referenceValue, newValue;

    if(random_factor() < chance)
        referenceValue = previousCalculations.back();

    else
        referenceValue = bestCalculation; ///@todo rename bestCalculation to bestOptValue or similar

    for(auto boundary = optBoundaries.cbegin(); boundary != optBoundaries.cend(); ++boundary)
    {
        ///@todo change logic could be a method
        T change, maxChange;

        maxChange = 0.5 * boundary->range() * temperature;
        change = random_factor() * maxChange;

        if(rand() % 2)
            change *= -1.0;

        newValue.add_parameter(boundary->name, referenceValue.get_parameter(boundary->name) + change);
    }

    update_temperature();
    update_chance();

    return newValue;
}

//------------------------------------------------------------------------------

OptValue OptSimulatedAnnealing::random_start_value()
{
    OptValue optValue;
    for(auto boundary = optBoundaries.cbegin(); boundary != optBoundaries.cend(); ++boundary)
    {
        T range = boundary->max - boundary->min;
        T newValue = boundary->min + random_factor() * range;
        optValue.add_parameter(boundary->name, newValue);
    }
    bestCalculation = optValue;
    bestCalculation.result = bad_value(); ///@todo bestCalculation logic should be moved to general OptBase (since it's gonna repeat itself)
    return optValue;
}

//------------------------------------------------------------------------------

void OptSimulatedAnnealing::update_temperature()
{
    temperature *= coolingFactor;
}

//------------------------------------------------------------------------------

void OptSimulatedAnnealing::update_chance()
{
    chance *= coolingFactor;
}

//------------------------------------------------------------------------------

} // namespace cppOpt
