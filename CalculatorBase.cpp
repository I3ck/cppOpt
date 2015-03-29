#include "CalculatorBase.h"

CalculatorBase::~CalculatorBase()
{

}

//------------------------------------------------------------------------------

///@todo move to OptBase or similar, since there has to be a mutex lock
void CalculatorBase::log_result(const std::string &pathLogFile, const OptValue &optValue) const
{
    std::ofstream of(pathLogFile.c_str(), std::ios_base::app);
    of << optValue.to_string() << std::endl;
    of.close();
}

