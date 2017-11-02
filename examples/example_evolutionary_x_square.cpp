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
  * \brief first example showing the usage of the evolutionary algorithm
  *        to find the minimum of x^2
  */

#include <iostream>
#include <cmath>

#include "cppOpt.h"

using namespace cppOpt;

//define your own calculation
auto toOptimize = [](OptCalculation<double>& optCalculation) {
    //defined x^2 as function to be optimised
    optCalculation.result = pow(optCalculation.get_parameter("X"),2);
};

using namespace std;

int main()
{
    //setup the optimisation
    //define your boundaries
    OptBoundaries<double> optBoundaries;
    optBoundaries.add_boundary({-5.0, 5.0, "X"});

    //number of calculations
    unsigned int maxCalculations = 300;

    //we want to find the minimum
    OptTarget optTarget = OptTarget::MINIMIZE;

    //how fast the evolutionary algorithm slows down
    //https://en.wikipedia.org/wiki/Evolutionary_algorithm
    double coolingFactor = 0.95;

    //how many individuals should be spawned in the beginning
    //use an even, positiv number
    //https://en.wikipedia.org/wiki/Evolutionary_algorithm
    unsigned int nIndividualsStart = 50;

    //how many individuals shall be selected each generation
    //this should also be an even number
    //https://en.wikipedia.org/wiki/Evolutionary_algorithm
    unsigned int nIndividualsSelection = 10;

    //how many children each parent pair should spawn
    //use a number > 2
    ////https://en.wikipedia.org/wiki/Evolutionary_algorithm
    unsigned int nIndividualsOffspring = 3;

    //how much the offspring should be mutated
    //or moved from the center of the parents
    //https://en.wikipedia.org/wiki/Evolutionary_algorithm
    double mutation = 0.1;

    //define your coordinator
    OptCoordinator<double, false> coordinator(
        maxCalculations,
        toOptimize,
        optTarget,
        0);

    //add evolutionary as child
    coordinator.add_child(make_unique<OptEvolutionary<double>>(
        optBoundaries,
        optTarget,
        0,
        coolingFactor,
        nIndividualsStart,
        nIndividualsSelection,
        nIndividualsOffspring,
        mutation));

    //let's go
    coordinator.run_optimisation(1);

    //print result
    OptCalculation<double> best = coordinator.get_best_calculation();
    cout << best.to_string_header() << endl;
    cout << best.to_string_values() << endl;

    return 0;
}
