#include "OptSimulatedAnnealing.h"

namespace cppOpt
{

OptSimulatedAnnealing::OptSimulatedAnnealing(const OptBoundaries &optBoundaries,
                                             unsigned int maxCalculations,
                                             OptSolverBase *pCalculator,
                                             OptTarget optTarget,
                                             OPT_T targetValue,
                                             OPT_T coolingFactor,
                                             OPT_T startChance) :
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

OptCalculation OptSimulatedAnnealing::get_next_value()
{
    if(previousCalculations.empty())
        return random_start_value();

    OptCalculation referenceValue, newValue;

    if(random_factor() < chance)
        referenceValue = previousCalculations.back();

    else
        referenceValue = bestCalculation; ///@todo rename bestCalculation to bestOptCalculation or similar

    while(true)
    {
        newValue = OptCalculation();
        for(auto boundary = optBoundaries.cbegin(); boundary != optBoundaries.cend(); ++boundary)
        {
            ///@todo change logic could be a method
            OPT_T change, maxChange;

            maxChange = 0.5 * boundary->range() * temperature;
            change = random_factor() * maxChange;

            if(rand() % 2)
                change *= -1.0;

            newValue.add_parameter(boundary->name, referenceValue.get_parameter(boundary->name) + change);
        }
        if(valid(newValue))
            break;
    }

    update_temperature();
    update_chance();

    return newValue;
}

//------------------------------------------------------------------------------

OptCalculation OptSimulatedAnnealing::random_start_value()
{
    OptCalculation optCalculation;
    for(auto boundary = optBoundaries.cbegin(); boundary != optBoundaries.cend(); ++boundary)
    {
        OPT_T range = boundary->max - boundary->min; ///@todo use range method here (and propaply in threshold accepting)
        OPT_T newValue = boundary->min + random_factor() * range;
        optCalculation.add_parameter(boundary->name, newValue);
    }
    bestCalculation = optCalculation;
    bestCalculation.result = bad_value(); ///@todo bestCalculation logic should be moved to general OptBase (since it's gonna repeat itself)
    return optCalculation;
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
