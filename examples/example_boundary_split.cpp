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
  * \brief example showing the usage of boundary splitting by using simulated annealing and threading
  *        to find the minimum of x^2
  */

#include <iostream>
#include <cmath>

#include "cppOpt.h"

using namespace cppOpt;

template <typename T>
class MySolver : public OptSolverBase<T>
{
public:
    //define your own calculation
    void calculate(OptCalculation<T> &optCalculation) const
    {
        //defined x^2 as function to be optimised
        optCalculation.result = pow(optCalculation.get_parameter("X"),2);
    }
};

using namespace std;

int main()
{
    //setup the optimisation
    //define your total boundaries
    OptBoundaries<double> optBoundariesTotal;
    optBoundariesTotal.add_boundary(-5.0, 5.0, "X");

    //and split them into 4
    vector< OptBoundaries<double> > optBoundariesSplit = optBoundariesTotal.split("X", 4);

    //instansiate your calculator
    MySolver<double> mySolver;

    //number of calculations
    unsigned int maxCalculations = 300;

    //we want to find the minimum
    OptTarget optTarget = MINIMIZE;

    //how fast the simulated annealing algorithm slows down
    //http://en.wikipedia.org/wiki/Simulated_annealing
    double coolingFactor = 0.9;

    //the chance in the beginning to follow bad solutions
    double startChance = 0.25;

    //create your optimiser
    //using simulated annealing
    //now creating 4 to run all at once
    OptSimulatedAnnealing<double> opt1(optBoundariesSplit[0],
                                       maxCalculations / 4,
                                       &mySolver,
                                       optTarget,
                                       0.0, //only required if approaching / diverging
                                       coolingFactor,
                                       startChance);

    OptSimulatedAnnealing<double> opt2(optBoundariesSplit[1],
                                       maxCalculations / 4,
                                       &mySolver,
                                       optTarget,
                                       0.0, //only required if approaching / diverging
                                       coolingFactor,
                                       startChance);

    OptSimulatedAnnealing<double> opt3(optBoundariesSplit[2],
                                       maxCalculations / 4,
                                       &mySolver,
                                       optTarget,
                                       0.0, //only required if approaching / diverging
                                       coolingFactor,
                                       startChance);

    OptSimulatedAnnealing<double> opt4(optBoundariesSplit[3],
                                       maxCalculations / 4,
                                       &mySolver,
                                       optTarget,
                                       0.0, //only required if approaching / diverging
                                       coolingFactor,
                                       startChance);

    //enable logging
    //boundaries object required to know the parameters names for the header
    OptBase<double>::enable_logging("example_boundary_split.log", optBoundariesSplit[0]);

    //let's go
    OptBase<double>::run_optimisations();

    //print result
    OptCalculation<double> best = OptBase<double>::get_best_calculation(optTarget, 0.0);
    cout << best.to_string_header() << endl;
    cout << best.to_string_values() << endl;

    return 0;
}
