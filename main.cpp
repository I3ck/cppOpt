#include <iostream>

#include "CalculatorBase.h"
#include "OptSimulatedAnnealing.h"

class MyCalculator : CalculatorBase
{
public:
    MyCalculator()
    {

    }

    void calculate(OptValue &optValue)
    {
        optValue.result = 5;
    }
};

using namespace std;

int main()
{
    std::vector<OptBoundary> optBoundaries;
    unsigned int maxCalculations = 100;
    OptTarget optTarget = MAXIMIZE;
    T coolingFactor = 0.9;
    T startChance = 0.25;
    OptBoundary optBoundary(3.0, 10.0, "X");
    optBoundaries.push_back(optBoundary);
    OptSimulatedAnnealing opt(optBoundaries, maxCalculations, optTarget, coolingFactor, startChance);



    MyCalculator myCalculator;
    OptValue optValue;
    std::map <std::string, T> params = {{"a",3}, {"b",4}};
    optValue.set_parameters(params);
    myCalculator.calculate(optValue);

    cout << opt.get_next_value().to_string() << endl;

    cout << optValue.result << endl;
    return 0;
}

