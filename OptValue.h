#ifndef OPTVALUE_H
#define OPTVALUE_H

#include <string>
#include <map>
#include <atomic>

#include "defines.h"


class OptValue
{
private:

    static std::atomic <unsigned int> maxId;

    unsigned int id;
    std::map <std::string, T> parameters;
    T result;


public:
    OptValue();
    ~OptValue();

    std::map <std::string, T> get_parameters() const;
    void set_parameters(const std::map <std::string, T> &value);

    T get_result() const;
    void set_result(const T &value);
};

#endif // OPTVALUE_H
