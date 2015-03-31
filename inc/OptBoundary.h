#ifndef OPTBOUNDARY_H
#define OPTBOUNDARY_H

#include <string>

#include "config.h"

namespace cppOpt
{

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

    T range() const;
};

} // namespace cppOpt

#endif // OPTBOUNDARY_H
