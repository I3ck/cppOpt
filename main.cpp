#include <iostream>

#include "CalculatorBase.h"

class MyCalculator : CalculatorBase
{
public:
    MyCalculator()
    {

    }

    void calculate(OptValue &optValue)
    {
        optValue.set_result(5);
    }
};

using namespace std;

int main()
{
    MyCalculator myCalculator;
    OptValue optValue;
    std::map <std::string, T> params = {{"a",3}, {"b",4}};
    optValue.set_parameters(params);
    myCalculator.calculate(optValue);


    cout << optValue.get_result() << endl;
    return 0;
}

