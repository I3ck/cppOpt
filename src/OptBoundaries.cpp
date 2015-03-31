#include "OptBoundaries.h"

namespace cppOpt
{

OptBoundaries::OptBoundaries()
{

}

//------------------------------------------------------------------------------

OptBoundaries::~OptBoundaries()
{

}

//------------------------------------------------------------------------------

void OptBoundaries::add_boundary(T min, T max, const std::string &name)
{
    OptBoundary temp(min, max, name);
    add_boundary(temp);
}

//------------------------------------------------------------------------------

void OptBoundaries::add_boundary(const OptBoundary &optBoundary)
{
    optBoundaries.push_back(optBoundary);
}

//------------------------------------------------------------------------------

std::string OptBoundaries::to_string() const
{
    std::string out("");

    for(const auto &boundary : optBoundaries)
        out += boundary.name + " ";

    return out;
}

//------------------------------------------------------------------------------

size_t OptBoundaries::size() const
{
    return optBoundaries.size();
}

//------------------------------------------------------------------------------

std::vector<OptBoundary>::iterator OptBoundaries::begin()
{
    return optBoundaries.begin();
}

//------------------------------------------------------------------------------

std::vector<OptBoundary>::iterator OptBoundaries::end()
{
    return optBoundaries.end();
}

//------------------------------------------------------------------------------

const std::vector<OptBoundary>::const_iterator OptBoundaries::cbegin() const
{
    return optBoundaries.cbegin();
}

//------------------------------------------------------------------------------

const std::vector<OptBoundary>::const_iterator OptBoundaries::cend() const
{
    return optBoundaries.cend();
}

//------------------------------------------------------------------------------

} // namespace cppOpt
