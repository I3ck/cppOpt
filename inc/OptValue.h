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

    static std::atomic <unsigned int> maxId;

    unsigned int id;
    std::map <std::string, T> parameters;

public:
    T result;

    OptValue();
    ~OptValue();

    ///@todo many members should be public / or const and defined in construction
    std::map <std::string, T> get_parameters() const;
    void set_parameters(const std::map <std::string, T> &value);
    void add_parameter(const std::string &name, T value);
    T get_parameter(const std::string &name) const;

    std::string to_string_values() const; ///@todo also add overloads for streams etc.
    std::string to_string_header() const; ///@todo also add overloads for streams etc.
};

} // namespace cppOpt

#endif // OPTVALUE_H
