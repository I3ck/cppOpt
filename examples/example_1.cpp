/*
    Copyright (c) 2015 Martin Buck
    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation the rights to
    use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
    and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
    The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
    DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
    OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/**
  * \brief first example showing the usage of simulated annealing
  *        to find the minimum of x^2
  */

#include <iostream>
#include <cmath>

#include "cppOpt.h"

using namespace cppOpt;

class MySolver : public OptSolverBase
{
public:
    //define your own calculation
    void calculate(OptCalculation &optCalculation) const
    {
        //defined x^2 as function to be optimised
        optCalculation.result = pow(optCalculation.get_parameter("X"),2);
    }
};

using namespace std;

int main()
{
    //setup the optimisation
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
    OPT_T coolingFactor = 0.95;

    //the chance in the beginning to follow bad solutions
    OPT_T startChance = 0.25;

    //create your optimiser
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
