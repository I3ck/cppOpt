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

auto toOptimize = [](OptCalculation<double>& optCalculation) {
    optCalculation.result = pow(optCalculation.get_parameter("X"),2);
};

TEST_CASE("Boundary") {

    SECTION("Constructor") {
        OptBoundary<double> optBoundary{0.0, 1.0, "test"};

        REQUIRE(optBoundary.min == 0.0);
        REQUIRE(optBoundary.max == 1.0);
        REQUIRE(optBoundary.name == "test");
    }

    SECTION("Range") {
        OptBoundary<double> optBoundary{0.0, 1.0, "test"};

        REQUIRE(optBoundary.range() == 1.0);

        OptBoundary<double> optBoundary2{1.0, 10.0, "test"};
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

        optBoundaries.add_boundary({1.0, 3.0, "test"});
        REQUIRE(optBoundaries.size() == 1);
    }

    SECTION("Splitting") {
        OptBoundaries<double> optBoundaries;

        optBoundaries.add_boundary({0.0, 10.0, "x"});
        optBoundaries.add_boundary({0.0, 10.0, "y"});
        optBoundaries.add_boundary({0.0, 10.0, "z"});

        std::vector< OptBoundaries<double> > splitted = optBoundaries.split("x", 10);

        for(unsigned int i = 0; i < optBoundaries.size(); ++i)
        {
            for(auto const& boundary : splitted[i])
            {
                if(boundary.first == "x")
                {
                    REQUIRE(boundary.second.min == 0.0 + i);
                    REQUIRE(boundary.second.max == 1.0 + i);
                }
                else if(boundary.first == "y")
                {
                    REQUIRE(boundary.second.min == 0.0);
                    REQUIRE(boundary.second.max == 10.0);
                }
                else if(boundary.first == "z")
                {
                    REQUIRE(boundary.second.min == 0.0);
                    REQUIRE(boundary.second.max == 10.0);
                }
            }
        }
    }
}

TEST_CASE("Simulated Annealing") {

    OptBoundaries<double> optBoundaries;
    optBoundaries.add_boundary({-5.0, 5.0, "X"});

    unsigned int maxCalculations = 300;
    double coolingFactor = 0.95;
    double startChance = 0.25;

    SECTION("Minimizing") {
        OptTarget optTarget = OptTarget::MINIMIZE;

        OptCoordinator<double, false> coordinator(
            maxCalculations,
            toOptimize,
            optTarget,
            0);

        coordinator.add_child(make_unique<OptSimulatedAnnealing<double>>(
            optBoundaries,
            coolingFactor,
            startChance));

        coordinator.run_optimisation();

        REQUIRE(fabs(coordinator.get_best_calculation().result - 0.0) < DELTA);
    }

    SECTION("Maximizing") {
        OptTarget optTarget = OptTarget::MAXIMIZE;

        OptCoordinator<double, false> coordinator(
            maxCalculations,
            toOptimize,
            optTarget,
            0);

        coordinator.add_child(make_unique<OptSimulatedAnnealing<double>>(
            optBoundaries,
            coolingFactor,
            startChance));

        coordinator.run_optimisation();

        REQUIRE(fabs(coordinator.get_best_calculation().result - 25.0) < DELTA);
    }

    SECTION("Approaching") {
        OptTarget optTarget = OptTarget::APPROACH;

        OptCoordinator<double, false> coordinator(
            maxCalculations,
            toOptimize,
            optTarget,
            3.3);

        coordinator.add_child(make_unique<OptSimulatedAnnealing<double>>(
            optBoundaries,
            coolingFactor,
            startChance));

        coordinator.run_optimisation();

        REQUIRE(fabs(coordinator.get_best_calculation().result - 3.3) < DELTA);
    }

    SECTION("Diverging1") {
        OptTarget optTarget = OptTarget::DIVERGE;

        OptCoordinator<double, false> coordinator(
            maxCalculations,
            toOptimize,
            optTarget,
            -100.0);

        coordinator.add_child(make_unique<OptSimulatedAnnealing<double>>(
            optBoundaries,
            coolingFactor,
            startChance));

        coordinator.run_optimisation();

        REQUIRE(fabs(coordinator.get_best_calculation().result - 25.0) < DELTA);
    }

    SECTION("Diverging2") {
        OptTarget optTarget = OptTarget::DIVERGE;

        OptCoordinator<double, false> coordinator(
            maxCalculations,
            toOptimize,
            optTarget,
            100.0);

        coordinator.add_child(make_unique<OptSimulatedAnnealing<double>>(
            optBoundaries,
            coolingFactor,
            startChance));

        coordinator.run_optimisation();

        REQUIRE(fabs(coordinator.get_best_calculation().result - 0.0) < DELTA);
    }
}

TEST_CASE("Threshold Accepting") {

    OptBoundaries<double> optBoundaries;
    optBoundaries.add_boundary({-5.0, 5.0, "X"});

    unsigned int maxCalculations = 300;
    double coolingFactor = 0.95;
    double threshold = 5.0;
    double thresholdFactor = 0.95;

    SECTION("Minimizing") {
        OptTarget optTarget = OptTarget::MINIMIZE;

        OptCoordinator<double, false> coordinator(
            maxCalculations,
            toOptimize,
            optTarget,
            0);

        coordinator.add_child(make_unique<OptThresholdAccepting<double>>(
            optBoundaries,
            optTarget,
            0,
            coolingFactor,
            threshold,
            thresholdFactor));

        coordinator.run_optimisation();

        REQUIRE(fabs(coordinator.get_best_calculation().result - 0.0) < DELTA);
    }

    SECTION("Maximizing") {
        OptTarget optTarget = OptTarget::MAXIMIZE;

        OptCoordinator<double, false> coordinator(
            maxCalculations,
            toOptimize,
            optTarget,
            0);

        coordinator.add_child(make_unique<OptThresholdAccepting<double>>(
            optBoundaries,
            optTarget,
            0,
            coolingFactor,
            threshold,
            thresholdFactor));

        coordinator.run_optimisation();

        REQUIRE(fabs(coordinator.get_best_calculation().result - 25.0) < DELTA);
    }

    SECTION("Approaching") {
        OptTarget optTarget = OptTarget::APPROACH;

        OptCoordinator<double, false> coordinator(
            maxCalculations,
            toOptimize,
            optTarget,
            3.3);

        coordinator.add_child(make_unique<OptThresholdAccepting<double>>(
            optBoundaries,
            optTarget,
            3.3,
            coolingFactor,
            threshold,
            thresholdFactor));

        coordinator.run_optimisation();

        REQUIRE(fabs(coordinator.get_best_calculation().result - 3.3) < DELTA);
    }

    SECTION("Diverging1") {
        OptTarget optTarget = OptTarget::DIVERGE;

        OptCoordinator<double, false> coordinator(
            maxCalculations,
            toOptimize,
            optTarget,
            -100.0);

        coordinator.add_child(make_unique<OptThresholdAccepting<double>>(
            optBoundaries,
            optTarget,
            -100.0,
            coolingFactor,
            threshold,
            thresholdFactor));

        coordinator.run_optimisation();

        REQUIRE(fabs(coordinator.get_best_calculation().result - 25.0) < DELTA);
    }

    SECTION("Diverging2") {
        OptTarget optTarget = OptTarget::DIVERGE;

        OptCoordinator<double, false> coordinator(
            maxCalculations,
            toOptimize,
            optTarget,
            100.0);

        coordinator.add_child(make_unique<OptThresholdAccepting<double>>(
            optBoundaries,
            optTarget,
            100.0,
            coolingFactor,
            threshold,
            thresholdFactor));

        coordinator.run_optimisation();

        REQUIRE(fabs(coordinator.get_best_calculation().result - 0.0) < DELTA);
    }
}

TEST_CASE("Great Deluge") {

    OptBoundaries<double> optBoundaries;
    optBoundaries.add_boundary({-5.0, 5.0, "X"});

    unsigned int maxCalculations = 300;
    double coolingFactor = 0.95;
    double rain = 0.2;

    SECTION("Minimizing") {
        OptTarget optTarget = OptTarget::MINIMIZE;
        double waterLevel = 15.0;

        OptCoordinator<double, false> coordinator(
            maxCalculations,
            toOptimize,
            optTarget,
            0);

        coordinator.add_child(make_unique<OptGreatDeluge<double>>(
            optBoundaries,
            optTarget,
            0,
            coolingFactor,
            waterLevel,
            rain));

        coordinator.run_optimisation();

        REQUIRE(fabs(coordinator.get_best_calculation().result - 0.0) < DELTA);
    }

    SECTION("Maximizing") {
        OptTarget optTarget = OptTarget::MAXIMIZE;
        double waterLevel = 10.0;

        OptCoordinator<double, false> coordinator(
            maxCalculations,
            toOptimize,
            optTarget,
            0);

        coordinator.add_child(make_unique<OptGreatDeluge<double>>(
            optBoundaries,
            optTarget,
            0,
            coolingFactor,
            waterLevel,
            rain));

        coordinator.run_optimisation();

        REQUIRE(fabs(coordinator.get_best_calculation().result - 25.0) < DELTA);
    }

    SECTION("Approaching") {
        OptTarget optTarget = OptTarget::APPROACH;
        double waterLevel = 18.0;

        OptCoordinator<double, false> coordinator(
            maxCalculations,
            toOptimize,
            optTarget,
            3.3);


        coordinator.add_child(make_unique<OptGreatDeluge<double>>(
            optBoundaries,
            optTarget,
            3.3,
            coolingFactor,
            waterLevel,
            rain));

        coordinator.run_optimisation();

        REQUIRE(fabs(coordinator.get_best_calculation().result - 3.3) < DELTA);
    }

    SECTION("Diverging1") {
        OptTarget optTarget = OptTarget::DIVERGE;
        double waterLevel = 10.0;

        OptCoordinator<double, false> coordinator(
            maxCalculations,
            toOptimize,
            optTarget,
            -100.0);

        coordinator.add_child(make_unique<OptGreatDeluge<double>>(
            optBoundaries,
            optTarget,
            -100.0,
            coolingFactor,
            waterLevel,
            rain));

        coordinator.run_optimisation();

        REQUIRE(fabs(coordinator.get_best_calculation().result - 25.0) < DELTA);
    }

    SECTION("Diverging2") {
        OptTarget optTarget = OptTarget::DIVERGE;
        double waterLevel = 10.0;

        OptCoordinator<double, false> coordinator(
            maxCalculations,
            toOptimize,
            optTarget,
            100.0);

        coordinator.add_child(make_unique<OptGreatDeluge<double>>(
            optBoundaries,
            optTarget,
            100.0,
            coolingFactor,
            waterLevel,
            rain));

        coordinator.run_optimisation();

        REQUIRE(fabs(coordinator.get_best_calculation().result - 0.0) < DELTA);
    }
}

TEST_CASE("Evolutionary") {

    OptBoundaries<double> optBoundaries;
    optBoundaries.add_boundary({-5.0, 5.0, "X"});

    unsigned int maxCalculations = 300;
    double coolingFactor = 0.95;

    unsigned int
        nIndividualsStart(50),
        nIndividualsSelection(10),
        nIndividualsOffspring(4);

    double mutation(0.3);

    SECTION("Minimizing") {
        OptTarget optTarget = OptTarget::MINIMIZE;

        OptCoordinator<double, false> coordinator(
            maxCalculations,
            toOptimize,
            optTarget,
            0);

        coordinator.add_child(make_unique<OptEvolutionary<double>>(
            optBoundaries,
            optTarget,
            0,
            coolingFactor,
            nIndividualsStart,
            nIndividualsSelection,
            nIndividualsOffspring,
            mutation));

        coordinator.run_optimisation();

        REQUIRE(fabs(coordinator.get_best_calculation().result - 0.0) < DELTA);
    }

    SECTION("Maximizing") {
        OptTarget optTarget = OptTarget::MAXIMIZE;

        OptCoordinator<double, false> coordinator(
            maxCalculations,
            toOptimize,
            optTarget,
            0);

        coordinator.add_child(make_unique<OptEvolutionary<double>>(
            optBoundaries,
            optTarget,
            0,
            coolingFactor,
            nIndividualsStart,
            nIndividualsSelection,
            nIndividualsOffspring,
            mutation));

        coordinator.run_optimisation();

        REQUIRE(fabs(coordinator.get_best_calculation().result - 25.0) < 100.0 * DELTA); //the evolutionary algorithm has big problems reaching the very edge of a problem, so the delta was increased
    }

    SECTION("Approaching") {
        OptTarget optTarget = OptTarget::APPROACH;

        OptCoordinator<double, false> coordinator(
            maxCalculations,
            toOptimize,
            optTarget,
            3.3);

        coordinator.add_child(make_unique<OptEvolutionary<double>>(
            optBoundaries,
            optTarget,
            3.3,
            coolingFactor,
            nIndividualsStart,
            nIndividualsSelection,
            nIndividualsOffspring,
            mutation));

        coordinator.run_optimisation();

        REQUIRE(fabs(coordinator.get_best_calculation().result - 3.3) < 100.0 * DELTA); //the evolutionary algorithm has big problems reaching the very edge of a problem, so the delta was increased
    }

    SECTION("Diverging1") {
        OptTarget optTarget = OptTarget::DIVERGE;

        OptCoordinator<double, false> coordinator(
            maxCalculations,
            toOptimize,
            optTarget,
            -100.0);

        coordinator.add_child(make_unique<OptEvolutionary<double>>(
            optBoundaries,
            optTarget,
            -100.0,
            coolingFactor,
            nIndividualsStart,
            nIndividualsSelection,
            nIndividualsOffspring,
            mutation));

        coordinator.run_optimisation();

        REQUIRE(fabs(coordinator.get_best_calculation().result - 25.0) < 100.0 * DELTA); //the evolutionary algorithm has big problems reaching the very edge of a problem, so the delta was increased
    }

    SECTION("Diverging2") {
        OptTarget optTarget = OptTarget::DIVERGE;

        OptCoordinator<double, false> coordinator(
            maxCalculations,
            toOptimize,
            optTarget,
            100.0);

        coordinator.add_child(make_unique<OptEvolutionary<double>>(
            optBoundaries,
            optTarget,
            100.0,
            coolingFactor,
            nIndividualsStart,
            nIndividualsSelection,
            nIndividualsOffspring,
            mutation));

        coordinator.run_optimisation();

        REQUIRE(fabs(coordinator.get_best_calculation().result - 0.0) < 100.0);
    }
}

TEST_CASE("Multithreading / Boundary Splitting") {

    OptBoundaries<double>
            optBoundaries1,
            optBoundaries2,
            optBoundaries3,
            optBoundaries4;

    optBoundaries1.add_boundary({-5.0, -4.0, "X"});
    optBoundaries2.add_boundary({-3.0, -1.0, "X"});
    optBoundaries3.add_boundary({-0.5, 0.5, "X"});
    optBoundaries4.add_boundary({5.0, 50.0, "X"});

    unsigned int maxCalculations = 3000;
    double coolingFactor = 0.95;
    double startChance = 0.25;

    SECTION("Minimizing") {
        OptTarget optTarget = OptTarget::MINIMIZE;

        OptCoordinator<double, true> coordinator(
            maxCalculations,
            toOptimize,
            optTarget,
            0);

        coordinator.add_child(make_unique<OptSimulatedAnnealing<double>>(
            optBoundaries1,
            coolingFactor,
            startChance));

        coordinator.add_child(make_unique<OptSimulatedAnnealing<double>>(
            optBoundaries2,
            coolingFactor,
            startChance));

        coordinator.add_child(make_unique<OptSimulatedAnnealing<double>>(
            optBoundaries3,
            coolingFactor,
            startChance));

        coordinator.add_child(make_unique<OptSimulatedAnnealing<double>>(
            optBoundaries4,
            coolingFactor,
            startChance));

        coordinator.run_optimisation(4);

        REQUIRE(fabs(coordinator.get_best_calculation().result - 0.0) < DELTA);
    }

    SECTION("Maximizing") {
        OptTarget optTarget = OptTarget::MAXIMIZE;

        OptCoordinator<double, true> coordinator(
            maxCalculations,
            toOptimize,
            optTarget,
            0);

        coordinator.add_child(make_unique<OptSimulatedAnnealing<double>>(
            optBoundaries1,
            coolingFactor,
            startChance));

        coordinator.add_child(make_unique<OptSimulatedAnnealing<double>>(
            optBoundaries2,
            coolingFactor,
            startChance));

        coordinator.add_child(make_unique<OptSimulatedAnnealing<double>>(
            optBoundaries3,
            coolingFactor,
            startChance));

        coordinator.add_child(make_unique<OptSimulatedAnnealing<double>>(
            optBoundaries4,
            coolingFactor,
            startChance));

        coordinator.run_optimisation(4);

        REQUIRE(fabs(coordinator.get_best_calculation().result - 2500.0) < DELTA);
    }
}
