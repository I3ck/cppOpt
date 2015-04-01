/**
  * \brief first example showing the usage of simulated annealing
  *        to find the minimum of x^2
  */

#include <iostream>
#include <cmath>

#include "cppOpt.h"

using namespace cppOpt;

class MySolver : public SolverBase
{
public:
    //define your own calculation
    void calculate(OptCalculation &optCalculation) const
    {
        //defined x^2 as function to be optimized
        optCalculation.result = pow(optCalculation.get_parameter("X"),2);
    }
};

using namespace std;

int main()
{
    //setup the optimization
    //define your boundaries
    OptBoundaries optBoundaries;
    optBoundaries.add_boundary(-5.0, 5.0, "X");

    //instansiate your calculator
    MySolver mySolver;

    //number of calculations
    unsigned int maxCalculations = 300;

    //we want to find the minimum
    OptTarget optTarget = MINIMIZE;

    //how fast the simulated annealing algorithm slows down
    //http://en.wikipedia.org/wiki/Simulated_annealing
    T coolingFactor = 0.95;

    //the chance in the beginning to follow bad solutions
    T startChance = 0.25;

    //create your optimizer
    //using simulated annealing
    OptSimulatedAnnealing opt(optBoundaries,
                              maxCalculations,
                              &mySolver,
                              optTarget,
                              0.0, //only required if approaching / diverging
                              coolingFactor,
                              startChance);

    //enable logging
    //boundaries object required to know the parameters names for the header
    OptBase::enable_logging("example_1.log", optBoundaries);

    //let's go
    OptBase::run_optimisations();

    //print result
    OptCalculation best = opt.get_best_calculation();
    cout << best.to_string_header() << endl;
    cout << best.to_string_values() << endl;

    return 0;
}
