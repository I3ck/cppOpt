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

#ifndef OPTCALCULATION_H
#define OPTCALCULATION_H

#include <string>
#include <map>
#include <atomic>
#include <cmath>

#include "config.h"

namespace cppOpt
{

template <typename T>
class OptCalculation
{
private:
    std::map <std::string, T> parameters;

public:
    T result;

//------------------------------------------------------------------------------

    OptCalculation()
    {

    }

//------------------------------------------------------------------------------

    ~OptCalculation()
    {

    }

//------------------------------------------------------------------------------

    std::map <std::string, T> get_parameters() const
    {
        return parameters;
    }

//------------------------------------------------------------------------------

    void set_parameters(const std::map <std::string, T> &value)
    {
        parameters = value;
    }

//------------------------------------------------------------------------------

    void add_parameter(const std::string &name, T value)
    {
        parameters[name] = value;
    }

//------------------------------------------------------------------------------

    bool has_parameter(const std::string &name) const
    {
        return (parameters.find(name) != parameters.end())
    }    
    
//------------------------------------------------------------------------------

    T get_parameter(const std::string &name) const
    {
        if(parameters.find(name) == parameters.end())
            throw std::runtime_error("Trying to access a non-existing parameter");
        return parameters.at(name);
    }

//------------------------------------------------------------------------------

    T distance_to(const OptCalculation &other) const
    {
        if(!using_same_parameters(other))
            throw std::runtime_error("Can't calculate the distance between two OptCalculation if they're not using the same parameters");

        T squareSum(0.0);
        for(const auto &parameter : parameters)
            squareSum += pow(parameters.at(parameter.first) - other.parameters.at(parameter.first), 2);

        return sqrt(squareSum);
    }

//------------------------------------------------------------------------------

    OptCalculation calculation_between(const OptCalculation &other) const
    {
        if(!using_same_parameters(other))
            throw std::runtime_error("Can't calculate the center of two OptCalculation not having them same parameters");

        OptCalculation out;

        for(const auto &parameter : parameters)
        {
            T centerValue = ( parameters.at(parameter.first) + other.parameters.at(parameter.first) ) / 2.0;
            out.add_parameter(parameter.first, centerValue);
        }

        return out;
    }

//------------------------------------------------------------------------------

    bool using_same_parameters(const OptCalculation &other) const
    {
        for(const auto &parameter : parameters)
        {
            if(other.parameters.find(parameter.first) == other.parameters.end())
                return false;
        }

        for(const auto &parameter : other.parameters)
        {
            if(parameters.find(parameter.first) == parameters.end())
                return false;
        }
        return true;
    }

//------------------------------------------------------------------------------

    std::string to_string_values(const std::string &delimiter = " ") const
    {
        std::string out("");

        for(const auto &parameter : parameters)
            out += std::to_string(parameter.second) + delimiter;

        out += std::to_string(result);

        return out;
    }

//------------------------------------------------------------------------------

    std::string to_string_header(const std::string &delimiter = " ") const
    {
        std::string out("");

        for(const auto &parameter : parameters)
            out += parameter.first + delimiter;

        out += "RESULT";

        return out;
    }

//------------------------------------------------------------------------------

};

} // namespace cppOpt

#endif // OPTCALCULATION_HS
