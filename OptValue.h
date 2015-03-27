#ifndef OPTVALUE_H
#define OPTVALUE_H

#include <string>
#include <map>
#include <atomic>

#include "defines.h"
//#include "OptBase.h"

//class OptBase; //forward reference


class OptValue
{
private:

    static std::atomic <unsigned int> maxId;

    unsigned int id;
    std::map <std::string, T> parameters;


    //OptBase *owner; ///@todo save this pointer in an external data container (e.g. pair <OptValue, OptBase*>)


public:
    T result;

    OptValue();
    ~OptValue();

    ///@todo many members should be public / or const and defined in construction
    std::map <std::string, T> get_parameters() const;
    void set_parameters(const std::map <std::string, T> &value);
    void add_parameter(const std::string &name, T value);
    T get_parameter(const std::string &name) const;

    //OptBase* get_owner() const;
    //void set_owner(OptBase *value);

    std::string to_string() const; ///@todo also add overloads for streams etc.
};

#endif // OPTVALUE_H
