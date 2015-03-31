#ifndef OPTVALUE_H
#define OPTVALUE_H

#include <string>
#include <map>
#include <atomic>

#include "config.h"

namespace cppOpt
{

class OptValue
{
private:
    std::map <std::string, T> parameters;

public:
    T result;

    OptValue();
    ~OptValue();

    std::map <std::string, T> get_parameters() const;
    void set_parameters(const std::map <std::string, T> &value);

    void add_parameter(const std::string &name, T value);
    T get_parameter(const std::string &name) const;

    std::string to_string_values() const;
    std::string to_string_header() const;
};

} // namespace cppOpt

#endif // OPTVALUE_H
