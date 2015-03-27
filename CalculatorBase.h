#ifndef CALCULATORBASE_H
#define CALCULATORBASE_H

#include "defines.h"
#include "OptValue.h"


class CalculatorBase
{
public:
    virtual ~CalculatorBase();
    virtual void calculate(OptValue &optValue) const = 0;
};

#endif // CALCULATORBASE_H
