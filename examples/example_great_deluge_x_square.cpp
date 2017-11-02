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
  * \brief first example showing the usage of great deluge
  *        to find the minimum of x^2
  */

#include <cmath>
#include <iostream>

#include "cppOpt.h"

using namespace cppOpt;

//define your own calculation
auto toOptimize = [](OptCalculation<double>& optCalculation) {
    //defined x^2 as function to be optimised
    optCalculation.result = pow(optCalculation.get_parameter("X"), 2);
};

using namespace std;

int main() {
    //setup the optimisation
    //define your boundaries
    OptBoundaries<double> optBoundaries;
    optBoundaries.add_boundary({-5.0, 5.0, "X"});

    //number of calculations
    unsigned int maxCalculations = 300;

    //we want to find the minimum
    OptTarget optTarget = OptTarget::MINIMIZE;

    //how fast the great deluge algorithm slows down
    //http://en.wikipedia.org/wiki/Great_Deluge_algorithm
    double coolingFactor = 0.95;

    //the initial water level
    //http://en.wikipedia.org/wiki/Great_Deluge_algorithm
    double waterLevel = 25.0;

    //how much rain is added to the water level per iteration
    //with x^2 from -5 to +5 the max value is 25, while the min and wanted value is 0
    //with 300 calculations, the water level should be pretty close to the optimum of 0
    //25/300 => 0.083333 [rain should AT LEAST be that much] => make it 0.15
    //http://en.wikipedia.org/wiki/Great_Deluge_algorithm
    double rain = 0.15;

    //define your coordinator
    OptCoordinator<double, false> coordinator(
        maxCalculations,
        toOptimize,
        optTarget,
        0);

    //add great deluge as child
    coordinator.add_child(make_unique<OptGreatDeluge<double>>(
        optBoundaries,
        optTarget,
        0,
        coolingFactor,
        waterLevel,
        rain));

    //let's go
    coordinator.run_optimisation();

    //print result
    OptCalculation<double> best = coordinator.get_best_calculation();
    cout << best.to_string_header() << endl;
    cout << best.to_string_values() << endl;

    return 0;
}
