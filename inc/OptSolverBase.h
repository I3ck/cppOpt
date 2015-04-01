#ifndef CALCULATORBASE_H
#define CALCULATORBASE_H

#include <fstream>

#include "config.h"
#include "OptCalculation.h"

namespace cppOpt
{

class OptSolverBase
{
public:
    virtual ~OptSolverBase();
    virtual void calculate(OptCalculation &optCalculation) const = 0;
};

} // namespace cppOpt

#endif // CALCULATORBASE_H
