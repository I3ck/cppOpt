#ifndef CALCULATORBASE_H
#define CALCULATORBASE_H

#include <fstream>

#include "defines.h"
#include "OptValue.h"

namespace cppOpt
{

class CalculatorBase
{
public:
    virtual ~CalculatorBase();
    virtual void calculate(OptValue &optValue) const = 0;
};

} // namespace cppOpt

#endif // CALCULATORBASE_H
