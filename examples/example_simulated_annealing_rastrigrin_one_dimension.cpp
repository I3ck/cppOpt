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
  * \brief example showing the usage of simulated annealing
  *        to find the minimum of the 1-dimensional rastrigrin function
  */

#include <cmath>
#include <iostream>

#include "cppOpt.h"

using namespace cppOpt;

//define your own calculation
auto toOptimize = [](OptCalculation<double>& optCalculation) {
    //defining the one-dimensional rastrigrin function
    double
        A(10.0),
        n(1.0),
        pi(3.1415926535897932384),
        x = optCalculation.get_parameter("x");

    double sum = x * x - A * cos(2.0 * pi * x);

    optCalculation.result = A * n + sum;
};

using namespace std;

int main() {
    //setup the optimisation
    //define your boundaries
    OptBoundaries<double> optBoundaries;
    optBoundaries.add_boundary({-5.12, 5.12, "x"});

    //number of calculations
    unsigned int maxCalculations = 3000;

    //we want to find the minimum
    OptTarget optTarget = OptTarget::MINIMIZE;

    //how fast the simulated annealing algorithm slows down
    //http://en.wikipedia.org/wiki/Simulated_annealing
    double coolingFactor = 0.99;

    //the chance in the beginning to follow bad solutions
    double startChance = 0.25;

    //define your coordinator
    OptCoordinator<double, false> coordinator(
        maxCalculations,
        toOptimize,
        optTarget,
        0);

    //add simulated annealing as child
    coordinator.add_child(make_unique<OptSimulatedAnnealing<double>>(
        optBoundaries,
        coolingFactor,
        startChance));

    //let's go
    coordinator.run_optimisation();

    //print result
    OptCalculation<double> best = coordinator.get_best_calculation();
    cout << best.to_string_header() << endl;
    cout << best.to_string_values() << endl;

    return 0;
}
