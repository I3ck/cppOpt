#include "OptValue.h"

std::atomic <unsigned int> OptValue::maxId(0); ///@todo unused?

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

    out += "\n";

    return out;
}

//------------------------------------------------------------------------------

std::string OptValue::to_string_header() const
{
    std::string out("");

    for(const auto &parameter : parameters)
        out += parameter.first + " ";

    out += "\n";

    return out;
}

//------------------------------------------------------------------------------

OptValue::OptValue()
{
    id = maxId;
    maxId++; ///@todo either add a way to reset these or don't make them static atomic and let the library user take care of it
}

//------------------------------------------------------------------------------

OptValue::~OptValue()
{

}

//------------------------------------------------------------------------------

