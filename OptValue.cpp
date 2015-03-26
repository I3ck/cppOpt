#include "OptValue.h"

std::atomic <unsigned int> OptValue::maxId(0);

std::map <std::string, T> OptValue::get_parameters() const
{
    return parameters;
}

void OptValue::set_parameters(const std::map <std::string, T> &value)
{
    parameters = value;
}

void OptValue::add_parameter(const std::string &name, T value)
{
    parameters[name] = value;
}

T OptValue::get_result() const
{
    return result;
}
void OptValue::set_result(const T &value)
{
    result = value;
}

OptBase* OptValue::get_owner() const
{
    return owner;
}
void OptValue::set_owner(OptBase *value)
{
    owner = value;
}

OptValue::OptValue()
{
    id = maxId;
    maxId++; ///@todo either add a way to reset these or don't make them static atomic and let the library user take care of it
}

OptValue::~OptValue()
{

}

