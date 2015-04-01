#include "OptBoundary.h"

namespace cppOpt
{

OptBoundary::OptBoundary(OPT_T min, OPT_T max, const std::string &name) :
    min(min),
    max(max),
    name(name)
{

}

//------------------------------------------------------------------------------

OptBoundary::~OptBoundary()
{

}

//------------------------------------------------------------------------------

OPT_T OptBoundary::range() const
{
    return max - min;
}

} // namespace cppOpt

