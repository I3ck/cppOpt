#ifndef OPTBOUNDARIES_H
#define OPTBOUNDARIES_H

#include <string>
#include <vector> ///@todo maybe switch to a list?
///@todo or use a map   map <BOUNDARY_NAME, BOUNDARY>

#include "defines.h"
#include "OptBoundary.h"

class OptBoundaries
{
private:
    std::vector<OptBoundary>
        optBoundaries;

public:
    OptBoundaries();
    ~OptBoundaries();

    void add_boundary(T min, T max, const std::string &name);
    void add_boundary(const OptBoundary &optBoundary);

    std::vector<OptBoundary>::iterator begin();
    std::vector<OptBoundary>::iterator end();

    const std::vector<OptBoundary>::const_iterator cbegin() const;
    const std::vector<OptBoundary>::const_iterator cend() const;
};
#endif // OPTBOUNDARIES_H
