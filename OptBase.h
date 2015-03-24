#ifndef OPTBASE_H
#define OPTBASE_H

#include <vector>

#include "defines.h"
#include "OptValue.h"


class OptBase
{
public:
    OptBase(const std::vector<OptValue> &optValues);
    ~OptBase();
};

#endif // OPTBASE_H
