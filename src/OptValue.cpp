#include "OptValue.h"

namespace cppOpt
{

//------------------------------------------------------------------------------

std::map <std::string, T> OptValue::get_parameters() const
{
    return parameters;
}

//------------------------------------------------------------------------------

void OptValue::set_parameters(const std::map <std::string, T> &value)
{
    parameters = value;
}

//------------------------------------------------------------------------------

void OptValue::add_parameter(const std::string &name, T value)
{
    parameters[name] = value;
}

//------------------------------------------------------------------------------

T OptValue::get_parameter(const std::string &name) const
{
    if(parameters.find(name) != parameters.end())
        return parameters.at(name); ///@todo these need proper error handling
    else return 0.0; ///@todo better error case
}

//------------------------------------------------------------------------------

std::string OptValue::to_string_values() const
{
    std::string out("");

    for(const auto &parameter : parameters)
        out += std::to_string(parameter.second) + " ";

    out += std::to_string(result);

    return out;
}

//------------------------------------------------------------------------------

std::string OptValue::to_string_header() const
{
    std::string out("");

    for(const auto &parameter : parameters)
        out += parameter.first + " ";

    out += "RESULT";

    return out;
}

//------------------------------------------------------------------------------

OptValue::OptValue()
{

}

//------------------------------------------------------------------------------

OptValue::~OptValue()
{

}

//------------------------------------------------------------------------------

} // namespace cppOpt
