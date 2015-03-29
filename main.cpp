#include <iostream>
#include <cmath> ///@todo only needed for example formula, maybe remove later

#include "CalculatorBase.h"
#include "OptSimulatedAnnealing.h"

class MyCalculator : public CalculatorBase
{
public:
    MyCalculator()
    {

    }

    void calculate(OptValue &optValue) const ///@todo make static for easier usage (may not work like this[static] or at least not help)
    {
#ifdef DEBUG
        std::cout << "DEBUG: in MyCalculator" << std::endl;
#endif
        optValue.result = pow(optValue.get_parameter("X"),2);
#ifdef DEBUG
        std::cout << "DEBUG: result: " << optValue.result << std::endl;
#endif
        log_result("log.test", optValue); ///@todo not thread safe yet
    }
};

using namespace std;

int main()
{
    OptBoundaries
        optBoundaries,
        optBoundaries2;
    unsigned int maxCalculations = 2;
    OptTarget optTarget = APPROACH;
    T coolingFactor = 0.995;
    T startChance = 0.25;
    OptBoundary optBoundary(-3.0, 10.0, "X");
    OptBoundary optBoundary2(-30.0, -10.0, "X");
    optBoundaries.add_boundary(optBoundary);
    optBoundaries2.add_boundary(optBoundary2);
    MyCalculator myCalculator;

    OptSimulatedAnnealing opt(optBoundaries, maxCalculations, &myCalculator, optTarget, 3.0, coolingFactor, startChance);
    OptSimulatedAnnealing opt2(optBoundaries2, maxCalculations, &myCalculator, optTarget, 3.0, coolingFactor, startChance);

    OptBase::run_optimisations(2);

    return 0;
}

