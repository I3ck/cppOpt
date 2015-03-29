#include "OptBoundaries.h"


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

const std::vector<OptBoundary>::iterator& OptBoundaries::cbegin()
{
    return optBoundaries.cbegin();
}

//------------------------------------------------------------------------------

const std::vector<OptBoundary>::iterator& OptBoundaries::cend()
{
    return optBoundaries.cend();
}

//------------------------------------------------------------------------------
