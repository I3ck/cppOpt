#ifndef CALCULATORBASE_H
#define CALCULATORBASE_H

#include <fstream>

#include "config.h"
#include "OptValue.h"

namespace cppOpt
{

class SolverBase
{
public:
    virtual ~SolverBase();
    virtual void calculate(OptValue &optValue) const = 0;
};

} // namespace cppOpt

#endif // CALCULATORBASE_H
