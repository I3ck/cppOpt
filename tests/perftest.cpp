/*
    Copyright (c) 2017 Martin Buck
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

#include <cmath>
#include <vector>
#include <iostream>
#include <ctime>

#include "cppOpt.h"

using namespace std;
using namespace cppOpt;

#define DELTA 0.01

auto toOptimize = [](OptCalculation<double>& optCalculation) {
    optCalculation.result = pow(optCalculation.get_parameter("X"),2) + pow(optCalculation.get_parameter("Y"),2) + 14.876;
};

double time_sa_1()
{
    auto start = clock();

    OptBoundaries<double> optBoundaries;
    optBoundaries.add_boundary(-5.0, 5.0, "X");
    optBoundaries.add_boundary(-14.0, 23.0, "Y");

    unsigned int maxCalculations = 30000;
    double coolingFactor = 0.95;
    double startChance = 0.25;
    OptTarget optTarget = MINIMIZE;

    OptSimulatedAnnealing<double> opt(
        optBoundaries,
        maxCalculations,
        toOptimize,
        optTarget,
        0.0,
        coolingFactor,
        startChance);

    OptBase<double>::run_optimisations();
    OptBase<double>::clear_results();

    return ( clock() - start ) / (double) CLOCKS_PER_SEC;
}

double time_sa_2()
{
    auto start = clock();

    OptBoundaries<double> optBoundaries;
    optBoundaries.add_boundary(-5.0, 5.0, "X");
    optBoundaries.add_boundary(-14.0, 23.0, "Y");

    unsigned int maxCalculations = 30000;
    double coolingFactor = 0.95;
    double startChance = 0.25;
    OptTarget optTarget = MINIMIZE;

    OptSimulatedAnnealing<double> opt1(
        optBoundaries,
        maxCalculations,
        toOptimize,
        optTarget,
        0.0,
        coolingFactor,
        startChance);

    OptSimulatedAnnealing<double> opt2(
        optBoundaries,
        maxCalculations,
        toOptimize,
        optTarget,
        0.0,
        coolingFactor,
        startChance);

    OptSimulatedAnnealing<double> opt3(
        optBoundaries,
        maxCalculations,
        toOptimize,
        optTarget,
        0.0,
        coolingFactor,
        startChance);

    OptBase<double>::run_optimisations();
    OptBase<double>::clear_results();

    return ( clock() - start ) / (double) CLOCKS_PER_SEC;
}

double n_times(int times, std::function<double(void)> const& f)
{
    double sum{0};
    for (int i = 0; i < times; ++i)
        sum += f();
    return sum/times;
}

int main()
{
    constexpr int times = 10;
    cout << "All times in seconds" << endl;
    cout << "time_sa_1() :\t " << n_times(times, time_sa_1) << endl;
    cout << "time_sa_2() :\t " << n_times(times, time_sa_2) << endl;
    return 0;
}
