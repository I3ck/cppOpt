#ifndef OPTBOUNDARY_H
#define OPTBOUNDARY_H

#include <string>

#include "config.h"

namespace cppOpt
{

class OptBoundary
{
public:

    const OPT_T
        min,
        max;

    const std::string
        name;

    OptBoundary(OPT_T min, OPT_T max, const std::string &name);
    ~OptBoundary();

    OPT_T range() const;
};

} // namespace cppOpt

#endif // OPTBOUNDARY_H
