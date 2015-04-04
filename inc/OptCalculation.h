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

class OptCalculation
{
private:
    std::map <std::string, OPT_T> parameters;

public:
    OPT_T result;

    OptCalculation();
    ~OptCalculation();

    std::map <std::string, OPT_T> get_parameters() const;
    void set_parameters(const std::map <std::string, OPT_T> &value);

    void add_parameter(const std::string &name, OPT_T value);
    OPT_T get_parameter(const std::string &name) const;

    ///@todo could be static
    OPT_T distance_to(const OptCalculation &other) const;

    ///@todo could be static
    OptCalculation calculation_between(const OptCalculation &other) const;

    std::string to_string_values(const std::string &delimiter = " ") const;
    std::string to_string_header(const std::string &delimiter = " ") const;
};

} // namespace cppOpt

#endif // OPTCALCULATION_HS
