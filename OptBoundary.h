#ifndef OPTBOUNDARY_H
#define OPTBOUNDARY_H

#include <string>

#include "defines.h"

class OptBoundary
{
public:

    const T
        min,
        max;

    const std::string
        name;

    OptBoundary(T min, T max, const std::string &name);
    ~OptBoundary();
};

#endif // OPTBOUNDARY_H
