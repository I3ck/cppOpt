#ifndef CALCULATORBASE_H
#define CALCULATORBASE_H

#include <fstream>

#include "defines.h"
#include "OptValue.h"


class CalculatorBase
{
public:
    virtual ~CalculatorBase();
    virtual void calculate(OptValue &optValue) const = 0;
    void log_result(const std::string &pathLogFile, const OptValue &optValue) const;
};

#endif // CALCULATORBASE_H
