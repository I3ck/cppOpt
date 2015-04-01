#include "OptCalculation.h"

namespace cppOpt
{

//------------------------------------------------------------------------------

std::map <std::string, T> OptCalculation::get_parameters() const
{
    return parameters;
}

//------------------------------------------------------------------------------

void OptCalculation::set_parameters(const std::map <std::string, T> &value)
{
    parameters = value;
}

//------------------------------------------------------------------------------

void OptCalculation::add_parameter(const std::string &name, T value)
{
    parameters[name] = value;
}

//------------------------------------------------------------------------------

T OptCalculation::get_parameter(const std::string &name) const
{
    if(parameters.find(name) != parameters.end())
        return parameters.at(name); ///@todo these need proper error handling
    else return 0.0; ///@todo better error case
}

//------------------------------------------------------------------------------

std::string OptCalculation::to_string_values() const
{
    std::string out("");

    for(const auto &parameter : parameters)
        out += std::to_string(parameter.second) + " ";

    out += std::to_string(result);

    return out;
}

//------------------------------------------------------------------------------

std::string OptCalculation::to_string_header() const
{
    std::string out("");

    for(const auto &parameter : parameters)
        out += parameter.first + " ";

    out += "RESULT";

    return out;
}

//------------------------------------------------------------------------------

OptCalculation::OptCalculation()
{

}

//------------------------------------------------------------------------------

OptCalculation::~OptCalculation()
{

}

//------------------------------------------------------------------------------

} // namespace cppOpt
