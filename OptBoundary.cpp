#include "OptBoundary.h"

namespace cppOpt
{

OptBoundary::OptBoundary(T min, T max, const std::string &name) :
    min(min),
    max(max),
    name(name)
{

}

//------------------------------------------------------------------------------

OptBoundary::~OptBoundary()
{

}

} // namespace cppOpt

