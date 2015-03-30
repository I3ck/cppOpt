/**
  * \brief second example showing the usage of simulated annealing
  *        to find the minimum of x^2
  *        but now using 4 threads
  */

#include <iostream>
#include <cmath>

#include "CalculatorBase.h"
#include "OptSimulatedAnnealing.h"

using namespace cppOpt;

class MyCalculator : public CalculatorBase
{
public:
    //define your own calculation
    void calculate(OptValue &optValue) const
    {
        //defined x^2 as function to be optimized
        optValue.result = pow(optValue.get_parameter("X"),2);
    }
};

using namespace std;

int main()
{
    //setup the optimization
    //define your boundaries
    //to enable 4 threads, the range of example_1 is split
    OptBoundaries
            optBoundaries1,
            optBoundaries2,
            optBoundaries3,
            optBoundaries4;

    optBoundaries1.add_boundary(-5.0, -2.5, "X");
    optBoundaries2.add_boundary(-2.5, 0.0, "X");
    optBoundaries3.add_boundary(0.0, 2.5, "X");
    optBoundaries4.add_boundary(2.5, 5.0, "X");

    //instansiate your calculator
    MyCalculator myCalculator;

    //number of calculations
    unsigned int maxCalculations = 300;
    
    //we want to find the minimum
    OptTarget optTarget = MINIMIZE;
    
    //how fast the simulated annealing algorithm slows down
    //http://en.wikipedia.org/wiki/Simulated_annealing
    T coolingFactor = 0.9;
    
    //the chance in the beginning to follow bad solutions
    T startChance = 0.25;

    //create your optimizer
    //using simulated annealing
    //now creating 4 to run all at once
    OptSimulatedAnnealing opt1(optBoundaries1,
                              maxCalculations / 4,
                              &myCalculator,
                              optTarget,
                              0.0, //only required if approaching / diverging
                              coolingFactor,
                              startChance);

    OptSimulatedAnnealing opt2(optBoundaries2,
                              maxCalculations / 4,
                              &myCalculator,
                              optTarget,
                              0.0, //only required if approaching / diverging
                              coolingFactor,
                              startChance);

    OptSimulatedAnnealing opt3(optBoundaries3,
                              maxCalculations / 4,
                              &myCalculator,
                              optTarget,
                              0.0, //only required if approaching / diverging
                              coolingFactor,
                              startChance);

    OptSimulatedAnnealing opt4(optBoundaries4,
                              maxCalculations / 4,
                              &myCalculator,
                              optTarget,
                              0.0, //only required if approaching / diverging
                              coolingFactor,
                              startChance);

    //enable logging
    //boundaries object required to know the parameters names for the header
    OptBase::enable_logging("example2.log", optBoundaries1);

    //let's go
    OptBase::run_optimisations();

    return 0;
}

