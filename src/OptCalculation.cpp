/*
    Copyright (c) 2015 Martin Buck
    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation the rights to
    use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
    and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
    The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
    DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
    OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "OptCalculation.h"

namespace cppOpt
{

//------------------------------------------------------------------------------

std::map <std::string, OPT_T> OptCalculation::get_parameters() const
{
    return parameters;
}

//------------------------------------------------------------------------------

void OptCalculation::set_parameters(const std::map <std::string, OPT_T> &value)
{
    parameters = value;
}

//------------------------------------------------------------------------------

void OptCalculation::add_parameter(const std::string &name, OPT_T value)
{
    parameters[name] = value;
}

//------------------------------------------------------------------------------

OPT_T OptCalculation::get_parameter(const std::string &name) const
{
    if(parameters.find(name) != parameters.end())
        return parameters.at(name); ///@todo these need proper error handling
    else return 0.0; ///@todo better error case
}

//------------------------------------------------------------------------------

OPT_T OptCalculation::distance_to(const OptCalculation &other) const
{
    ///@todo add a test to check whether both have the exact same parameters
    ///@todo if not return the max value for OPT_T
    OPT_T squareSum(0.0);
    for(const auto &parameter : parameters)
        squareSum += pow(parameters[parameter] - other.parameters[parameter], 2);

    return sqrt(squareSum);
}

//------------------------------------------------------------------------------

std::string OptCalculation::to_string_values(const std::string &delimiter) const
{
    std::string out("");

    for(const auto &parameter : parameters)
        out += std::to_string(parameter.second) + delimiter;

    out += std::to_string(result);

    return out;
}

//------------------------------------------------------------------------------

std::string OptCalculation::to_string_header(const std::string &delimiter) const
{
    std::string out("");

    for(const auto &parameter : parameters)
        out += parameter.first + delimiter;

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
