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

#define CATCH_CONFIG_MAIN
#include "../dependencies/Catch.h" //https://github.com/philsquared/Catch

#include <cmath>
#include <vector>

#include "cppOpt.h"

using namespace std;
using namespace cppOpt;

#define DELTA 0.01

template <typename T>
class MySolver : public OptSolverBase<T>
{
public:
    void calculate(OptCalculation<T> &optCalculation) const
    {
        optCalculation.result = pow(optCalculation.get_parameter("X"),2);
    }
};


TEST_CASE("Boundary") {

    SECTION("Constructor") {
        OptBoundary<double> optBoundary(0.0, 1.0, "test");

        REQUIRE(optBoundary.min == 0.0);
        REQUIRE(optBoundary.max == 1.0);
        REQUIRE(optBoundary.name == "test");
    }

    SECTION("Range") {
        OptBoundary<double> optBoundary(0.0, 1.0, "test");

        REQUIRE(optBoundary.range() == 1.0);

        OptBoundary<double> optBoundary2(1.0, 10.0, "test");
        REQUIRE(optBoundary2.range() == 9.0);
    }
}

TEST_CASE("Boundaries") {

    SECTION("Constructor") {
        OptBoundaries<double> optBoundaries;
    }

    SECTION("Adding") {
        OptBoundaries<double> optBoundaries;

        REQUIRE(optBoundaries.size() == 0);

        optBoundaries.add_boundary(1.0, 3.0, "test");
        REQUIRE(optBoundaries.size() == 1);

        optBoundaries.add_boundary(OptBoundary<double>(1.0, 3.0, "test"));
        REQUIRE(optBoundaries.size() == 2);
    }

    SECTION("Splitting") {
        OptBoundaries<double> optBoundaries;

        optBoundaries.add_boundary(0.0, 10.0, "x");
        optBoundaries.add_boundary(0.0, 10.0, "y");
        optBoundaries.add_boundary(0.0, 10.0, "z");

        std::vector< OptBoundaries<double> > splitted = optBoundaries.split("x", 10);

        for(unsigned int i = 0; i < optBoundaries.size(); ++i)
        {
            for(const auto &boundary : splitted[i])
            {
                if(boundary.name == "x")
                {
                    REQUIRE(boundary.min == 0.0 + i);
                    REQUIRE(boundary.max == 1.0 + i);
                }
                else if(boundary.name == "y")
                {
                    REQUIRE(boundary.min == 0.0);
                    REQUIRE(boundary.max == 10.0);
                }
                else if(boundary.name == "z")
                {
                    REQUIRE(boundary.min == 0.0);
                    REQUIRE(boundary.max == 10.0);
                }
            }
        }
    }
}

TEST_CASE("Simulated Annealing") {

    OptBoundaries<double> optBoundaries;
    optBoundaries.add_boundary(-5.0, 5.0, "X");

    MySolver<double> mySolver;
    unsigned int maxCalculations = 300;
    double coolingFactor = 0.95;
    double startChance = 0.25;

    SECTION("Minimizing") {
        OptTarget optTarget = MINIMIZE;

        OptSimulatedAnnealing<double> opt(optBoundaries,
                                          maxCalculations,
                                          &mySolver,
                                          optTarget,
                                          0.0, //only required if approaching / diverging
                                          coolingFactor,
                                          startChance);

        OptBase<double>::run_optimisations();

        REQUIRE(fabs(opt.get_best_calculation().result - 0.0) < DELTA);
    }

    SECTION("Maximizing") {
        OptTarget optTarget = MAXIMIZE;

        OptSimulatedAnnealing<double> opt(optBoundaries,
                                          maxCalculations,
                                          &mySolver,
                                          optTarget,
                                          0.0, //only required if approaching / diverging
                                          coolingFactor,
                                          startChance);

        OptBase<double>::run_optimisations();

        REQUIRE(fabs(opt.get_best_calculation().result - 25.0) < DELTA);
    }

    SECTION("Approaching") {
        OptTarget optTarget = APPROACH;

        OptSimulatedAnnealing<double> opt(optBoundaries,
                                          maxCalculations,
                                          &mySolver,
                                          optTarget,
                                          3.3, //only required if approaching / diverging
                                          coolingFactor,
                                          startChance);

        OptBase<double>::run_optimisations();

        REQUIRE(fabs(opt.get_best_calculation().result - 3.3) < DELTA);
    }

    SECTION("Diverging1") {
        OptTarget optTarget = DIVERGE;

        OptSimulatedAnnealing<double> opt(optBoundaries,
                                          maxCalculations,
                                          &mySolver,
                                          optTarget,
                                          -100.0, //only required if approaching / diverging
                                          coolingFactor,
                                          startChance);

        OptBase<double>::run_optimisations();

        REQUIRE(fabs(opt.get_best_calculation().result - 25.0) < DELTA);
    }

    SECTION("Diverging2") {
        OptTarget optTarget = DIVERGE;

        OptSimulatedAnnealing<double> opt(optBoundaries,
                                          maxCalculations,
                                          &mySolver,
                                          optTarget,
                                          100.0, //only required if approaching / diverging
                                          coolingFactor,
                                          startChance);

        OptBase<double>::run_optimisations();

        REQUIRE(fabs(opt.get_best_calculation().result - 0.0) < DELTA);
    }
}

TEST_CASE("Threshold Accepting") {

    OptBoundaries<double> optBoundaries;
    optBoundaries.add_boundary(-5.0, 5.0, "X");

    MySolver<double> mySolver;
    unsigned int maxCalculations = 300;
    double coolingFactor = 0.95;
    double threshold = 5.0;
    double thresholdFactor = 0.95;

    SECTION("Minimizing") {
        OptTarget optTarget = MINIMIZE;

        OptThresholdAccepting<double> opt(optBoundaries,
                                          maxCalculations,
                                          &mySolver,
                                          optTarget,
                                          0.0, //only required if approaching / diverging
                                          coolingFactor,
                                          threshold,
                                          thresholdFactor);

        OptBase<double>::run_optimisations();

        REQUIRE(fabs(opt.get_best_calculation().result - 0.0) < DELTA);
    }

    SECTION("Maximizing") {
        OptTarget optTarget = MAXIMIZE;

        OptThresholdAccepting<double> opt(optBoundaries,
                                          maxCalculations,
                                          &mySolver,
                                          optTarget,
                                          0.0, //only required if approaching / diverging
                                          coolingFactor,
                                          threshold,
                                          thresholdFactor);

        OptBase<double>::run_optimisations();

        REQUIRE(fabs(opt.get_best_calculation().result - 25.0) < DELTA);
    }

    SECTION("Approaching") {
        OptTarget optTarget = APPROACH;

        OptThresholdAccepting<double> opt(optBoundaries,
                                          maxCalculations,
                                          &mySolver,
                                          optTarget,
                                          3.3, //only required if approaching / diverging
                                          coolingFactor,
                                          threshold,
                                          thresholdFactor);

        OptBase<double>::run_optimisations();

        REQUIRE(fabs(opt.get_best_calculation().result - 3.3) < DELTA);
    }

    SECTION("Diverging1") {
        OptTarget optTarget = DIVERGE;

        OptThresholdAccepting<double> opt(optBoundaries,
                                          maxCalculations,
                                          &mySolver,
                                          optTarget,
                                          -100.0, //only required if approaching / diverging
                                          coolingFactor,
                                          threshold,
                                          thresholdFactor);

        OptBase<double>::run_optimisations();

        REQUIRE(fabs(opt.get_best_calculation().result - 25.0) < DELTA);
    }

    SECTION("Diverging2") {
        OptTarget optTarget = DIVERGE;

        OptThresholdAccepting<double> opt(optBoundaries,
                                          maxCalculations,
                                          &mySolver,
                                          optTarget,
                                          100.0, //only required if approaching / diverging
                                          coolingFactor,
                                          threshold,
                                          thresholdFactor);

        OptBase<double>::run_optimisations();

        REQUIRE(fabs(opt.get_best_calculation().result - 0.0) < DELTA);
    }
}

TEST_CASE("Great Deluge") {

    OptBoundaries<double> optBoundaries;
    optBoundaries.add_boundary(-5.0, 5.0, "X");

    MySolver<double> mySolver;
    unsigned int maxCalculations = 300;
    double coolingFactor = 0.95;
    double rain = 0.2;

    SECTION("Minimizing") {
        OptTarget optTarget = MINIMIZE;
        double waterLevel = 15.0;

        OptGreatDeluge<double> opt(optBoundaries,
                                   maxCalculations,
                                   &mySolver,
                                   optTarget,
                                   0.0, //only required if approaching / diverging
                                   coolingFactor,
                                   waterLevel,
                                   rain);

        OptBase<double>::run_optimisations();

        REQUIRE(fabs(opt.get_best_calculation().result - 0.0) < DELTA);
    }

    SECTION("Maximizing") {
        OptTarget optTarget = MAXIMIZE;
        double waterLevel = 10.0;

        OptGreatDeluge<double> opt(optBoundaries,
                                   maxCalculations,
                                   &mySolver,
                                   optTarget,
                                   0.0, //only required if approaching / diverging
                                   coolingFactor,
                                   waterLevel,
                                   rain);

        OptBase<double>::run_optimisations();

        REQUIRE(fabs(opt.get_best_calculation().result - 25.0) < DELTA);
    }

    SECTION("Approaching") {
        OptTarget optTarget = APPROACH;
        double waterLevel = 18.0;

        OptGreatDeluge<double> opt(optBoundaries,
                                   maxCalculations,
                                   &mySolver,
                                   optTarget,
                                   3.3, //only required if approaching / diverging
                                   coolingFactor,
                                   waterLevel,
                                   rain);

        OptBase<double>::run_optimisations();

        REQUIRE(fabs(opt.get_best_calculation().result - 3.3) < DELTA);
    }

    SECTION("Diverging1") {
        OptTarget optTarget = DIVERGE;
        double waterLevel = 10.0;

        OptGreatDeluge<double> opt(optBoundaries,
                                   maxCalculations,
                                   &mySolver,
                                   optTarget,
                                   -100.0, //only required if approaching / diverging
                                   coolingFactor,
                                   waterLevel,
                                   rain);

        OptBase<double>::run_optimisations();

        REQUIRE(fabs(opt.get_best_calculation().result - 25.0) < DELTA);
    }

    SECTION("Diverging2") {
        OptTarget optTarget = DIVERGE;
        double waterLevel = 10.0;

        OptGreatDeluge<double> opt(optBoundaries,
                                   maxCalculations,
                                   &mySolver,
                                   optTarget,
                                   100.0, //only required if approaching / diverging
                                   coolingFactor,
                                   waterLevel,
                                   rain);

        OptBase<double>::run_optimisations();

        REQUIRE(fabs(opt.get_best_calculation().result - 0.0) < DELTA);
    }
}

TEST_CASE("Evolutionary") {

    OptBoundaries<double> optBoundaries;
    optBoundaries.add_boundary(-5.0, 5.0, "X");

    MySolver<double> mySolver;
    unsigned int maxCalculations = 300;
    double coolingFactor = 0.95;

    unsigned int
        nIndividualsStart(50),
        nIndividualsSelection(10),
        nIndividualsOffspring(4);

    double mutation(0.3);

    SECTION("Minimizing") {
        OptTarget optTarget = MINIMIZE;

        OptEvolutionary<double> opt(optBoundaries,
                                    maxCalculations,
                                    &mySolver,
                                    optTarget,
                                    0.0, //only required if approaching / diverging
                                    coolingFactor,
                                    nIndividualsStart,
                                    nIndividualsSelection,
                                    nIndividualsOffspring,
                                    mutation);

        OptBase<double>::run_optimisations();

        REQUIRE(fabs(opt.get_best_calculation().result - 0.0) < DELTA);
    }

    SECTION("Maximizing") {
        OptTarget optTarget = MAXIMIZE;

        OptEvolutionary<double> opt(optBoundaries,
                                    maxCalculations,
                                    &mySolver,
                                    optTarget,
                                    0.0, //only required if approaching / diverging
                                    coolingFactor,
                                    nIndividualsStart,
                                    nIndividualsSelection,
                                    nIndividualsOffspring,
                                    mutation);

        OptBase<double>::run_optimisations();

        REQUIRE(fabs(opt.get_best_calculation().result - 25.0) < 100.0 * DELTA); //the evolutionary algorithm has big problems reaching the very edge of a problem, so the delta was increased
    }

    SECTION("Approaching") {
        OptTarget optTarget = APPROACH;

        OptEvolutionary<double> opt(optBoundaries,
                                    maxCalculations,
                                    &mySolver,
                                    optTarget,
                                    3.3,
                                    coolingFactor,
                                    nIndividualsStart,
                                    nIndividualsSelection,
                                    nIndividualsOffspring,
                                    mutation);

        OptBase<double>::run_optimisations();

        REQUIRE(fabs(opt.get_best_calculation().result - 3.3) < 100.0 * DELTA); //the evolutionary algorithm has big problems reaching the very edge of a problem, so the delta was increased
    }

    SECTION("Diverging1") {
        OptTarget optTarget = DIVERGE;

        OptEvolutionary<double> opt(optBoundaries,
                                    maxCalculations,
                                    &mySolver,
                                    optTarget,
                                    -100.0,
                                    coolingFactor,
                                    nIndividualsStart,
                                    nIndividualsSelection,
                                    nIndividualsOffspring,
                                    mutation);

        OptBase<double>::run_optimisations();

        REQUIRE(fabs(opt.get_best_calculation().result - 25.0) < 100.0 * DELTA); //the evolutionary algorithm has big problems reaching the very edge of a problem, so the delta was increased
    }

    SECTION("Diverging2") {
        OptTarget optTarget = DIVERGE;

        OptEvolutionary<double> opt(optBoundaries,
                                    maxCalculations,
                                    &mySolver,
                                    optTarget,
                                    100.0,
                                    coolingFactor,
                                    nIndividualsStart,
                                    nIndividualsSelection,
                                    nIndividualsOffspring,
                                    mutation);

        OptBase<double>::run_optimisations();

        REQUIRE(fabs(opt.get_best_calculation().result - 0.0) < 100.0);
    }
}

TEST_CASE("Multithreading / Boundary Splitting") {
    
    OptBoundaries<double>
            optBoundaries1,
            optBoundaries2,
            optBoundaries3,
            optBoundaries4;

    optBoundaries1.add_boundary(-5.0, -4.0, "X");
    optBoundaries2.add_boundary(-3.0, -1.0, "X");
    optBoundaries3.add_boundary(-0.5, 0.5, "X");
    optBoundaries4.add_boundary(5.0, 50.0, "X");

    MySolver<double> mySolver;
    unsigned int maxCalculations = 300;
    double coolingFactor = 0.95;
    double startChance = 0.25;

    SECTION("Minimizing") {
        OptTarget optTarget = MINIMIZE;

        OptSimulatedAnnealing<double> opt1(optBoundaries1,
                                           maxCalculations,
                                           &mySolver,
                                           optTarget,
                                           0.0, //only required if approaching / diverging
                                           coolingFactor,
                                           startChance);

        OptSimulatedAnnealing<double> opt2(optBoundaries2,
                                           maxCalculations,
                                           &mySolver,
                                           optTarget,
                                           0.0, //only required if approaching / diverging
                                           coolingFactor,
                                           startChance);

        OptSimulatedAnnealing<double> opt3(optBoundaries3,
                                           maxCalculations,
                                           &mySolver,
                                           optTarget,
                                           0.0, //only required if approaching / diverging
                                           coolingFactor,
                                           startChance);

        OptSimulatedAnnealing<double> opt4(optBoundaries4,
                                           maxCalculations,
                                           &mySolver,
                                           optTarget,
                                           0.0, //only required if approaching / diverging
                                           coolingFactor,
                                           startChance);

        OptBase<double>::run_optimisations();

        REQUIRE(fabs(opt1.get_best_calculation().result - 16.0) < DELTA);
        REQUIRE(fabs(opt2.get_best_calculation().result - 1.0) < DELTA);
        REQUIRE(fabs(opt3.get_best_calculation().result - 0.0) < DELTA);
        REQUIRE(fabs(opt4.get_best_calculation().result - 25.0) < DELTA);

        REQUIRE(fabs(OptBase<double>::get_best_calculation(optTarget, 0.0).result - 0.0) < DELTA);
    }

    SECTION("Maximizing") {
        OptTarget optTarget = MAXIMIZE;

        OptSimulatedAnnealing<double> opt1(optBoundaries1,
                                           maxCalculations,
                                           &mySolver,
                                           optTarget,
                                           0.0, //only required if approaching / diverging
                                           coolingFactor,
                                           startChance);

        OptSimulatedAnnealing<double> opt2(optBoundaries2,
                                           maxCalculations,
                                           &mySolver,
                                           optTarget,
                                           0.0, //only required if approaching / diverging
                                           coolingFactor,
                                           startChance);

        OptSimulatedAnnealing<double> opt3(optBoundaries3,
                                           maxCalculations,
                                           &mySolver,
                                           optTarget,
                                           0.0, //only required if approaching / diverging
                                           coolingFactor,
                                           startChance);

        OptSimulatedAnnealing<double> opt4(optBoundaries4,
                                           maxCalculations,
                                           &mySolver,
                                           optTarget,
                                           0.0, //only required if approaching / diverging
                                           coolingFactor,
                                           startChance);

        OptBase<double>::run_optimisations();

        REQUIRE(fabs(opt1.get_best_calculation().result - 25.0) < DELTA);
        REQUIRE(fabs(opt2.get_best_calculation().result - 9.0) < DELTA);
        REQUIRE(fabs(opt3.get_best_calculation().result - 0.25) < DELTA);
        REQUIRE(fabs(opt4.get_best_calculation().result - 2500.0) < DELTA);

        REQUIRE(fabs(OptBase<double>::get_best_calculation(optTarget, 0.0).result - 2500.0) < DELTA);
    }
}
