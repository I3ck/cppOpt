#ifndef OPTCALCULATION_H
#define OPTCALCULATION_H

#include <string>
#include <map>
#include <atomic>

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

    std::string to_string_values(const std::string &delimiter = " ") const;
    std::string to_string_header(const std::string &delimiter = " ") const;
};

} // namespace cppOpt

#endif // OPTCALCULATION_HS
