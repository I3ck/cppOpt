#include "OptThresholdAccepting.h"

namespace cppOpt
{

OptThresholdAccepting::OptThresholdAccepting(const OptBoundaries &optBoundaries,
                                             unsigned int maxCalculations,
                                             SolverBase* pCalculator,
                                             OptTarget optTarget,
                                             T targetValue,
                                             T coolingFactor,
                                             T threshold,
                                             T thresholdFactor) :
    OptBase(optBoundaries, maxCalculations, pCalculator, optTarget, targetValue),
    temperature(1.0),
    coolingFactor(coolingFactor),
    threshold(threshold),
    thresholdFactor(thresholdFactor)
{

}

//------------------------------------------------------------------------------

OptThresholdAccepting::~OptThresholdAccepting()
{

}

//------------------------------------------------------------------------------

OptValue OptThresholdAccepting::get_next_value()
{
    if(previousCalculations.empty())
        return random_start_value();

    OptValue newValue;

    if(previousCalculations.size() == 1)
    {
        optValueReference = previousCalculations[0];
        optValueConfigurationC = previousCalculations[0];

        while(true)
        {
            newValue = OptValue();
            for(auto boundary = optBoundaries.cbegin(); boundary != optBoundaries.cend(); ++boundary)
            {
                ///@todo change logic could be a method
                T change, maxChange;

                maxChange = 0.5 * boundary->range() * temperature;
                change = random_factor() * maxChange;

                if(rand() % 2)
                    change *= -1.0;

                newValue.add_parameter(boundary->name, previousCalculations[0].get_parameter(boundary->name) + change);
            }
            if(valid(newValue))
                break;
        }

        update_temperature();
        update_threshold();

        return newValue;
    }

    OptValue center;

    if(result_better(previousCalculations.back(), optValueReference, optTarget))
        optValueReference = previousCalculations.back();

    OptValue compareValue = compare_value();

    if(result_better(previousCalculations.back(), compareValue, optTarget))
        optValueConfigurationC = previousCalculations.back();

    center = optValueConfigurationC;

    while(true)
    {
        newValue = OptValue();
        for(auto boundary = optBoundaries.cbegin(); boundary != optBoundaries.cend(); ++boundary)
        {
            ///@todo change logic could be a method
            T change, maxChange;

            maxChange = 0.5 * boundary->range() * temperature;
            change = random_factor() * maxChange;

            if(rand() % 2)
                change *= -1.0;

            newValue.add_parameter(boundary->name, center.get_parameter(boundary->name) + change);
        }
        if(valid(newValue))
            break;
    }

    update_temperature();
    update_threshold();
    return newValue;
}

//------------------------------------------------------------------------------

OptValue OptThresholdAccepting::random_start_value()
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

void OptThresholdAccepting::update_temperature()
{
    temperature *= coolingFactor;
}

//------------------------------------------------------------------------------

void OptThresholdAccepting::update_threshold()
{
    threshold *= thresholdFactor;
}

//------------------------------------------------------------------------------

OptValue OptThresholdAccepting::compare_value() const
{
    OptValue out;
    switch(optTarget)
    {
        case MINIMIZE:
            out.result = optValueConfigurationC.result + threshold;
            break;

        case MAXIMIZE:
            out.result = optValueConfigurationC.result - threshold;
            break;

        case APPROACH:
            if(targetValue > optValueConfigurationC.result)
                out.result = optValueConfigurationC.result - threshold;
            else
                out.result = optValueConfigurationC.result + threshold;
            break;

        case DIVERGE:
            if(targetValue > optValueConfigurationC.result)
                out.result = optValueConfigurationC.result + threshold;
            else
                out.result = optValueConfigurationC.result - threshold;
            break;

        default: // MINIMIZE
            out.result = optValueConfigurationC.result + threshold;
    }
    return out;
}

//------------------------------------------------------------------------------

} // namespace cppOpt
