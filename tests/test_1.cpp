#define CATCH_CONFIG_MAIN
#include "../dependencies/Catch.h" //https://github.com/philsquared/Catch

#include <cmath>

#include "OptBoundary.h"
#include "OptBoundaries.h"
#include "SolverBase.h"
#include "OptSimulatedAnnealing.h"
#include "OptThresholdAccepting.h"


using namespace std;
using namespace cppOpt;

#define DELTA 0.01


TEST_CASE("Boundary") {

    SECTION("Constructor") {
        OptBoundary optBoundary(0.0, 1.0, "test");

        REQUIRE(optBoundary.min == 0.0);
        REQUIRE(optBoundary.max == 1.0);
        REQUIRE(optBoundary.name == "test");
    }

    SECTION("Range") {
        OptBoundary optBoundary(0.0, 1.0, "test");

        REQUIRE(optBoundary.range() == 1.0);

        OptBoundary optBoundary2(1.0, 10.0, "test");
        REQUIRE(optBoundary2.range() == 9.0);
    }
}

TEST_CASE("Boundaries") {

    SECTION("Constructor") {
        OptBoundaries optBoundaries;
    }

    SECTION("Adding") {
        OptBoundaries optBoundaries;

        REQUIRE(optBoundaries.size() == 0);

        optBoundaries.add_boundary(1.0, 3.0, "test");
        REQUIRE(optBoundaries.size() == 1);

        optBoundaries.add_boundary(OptBoundary(1.0, 3.0, "test"));
        REQUIRE(optBoundaries.size() == 2);
    }
}

TEST_CASE("Simulated Annealing") {
    class MySolver : public SolverBase
    {
    public:
        void calculate(OptCalculation &optCalculation) const
        {
            optCalculation.result = pow(optCalculation.get_parameter("X"),2);
        }
    };

    OptBoundaries optBoundaries;
    optBoundaries.add_boundary(-5.0, 5.0, "X");

    MySolver myCalculator;
    unsigned int maxCalculations = 300;
    OPT_T coolingFactor = 0.95;
    OPT_T startChance = 0.25;

    SECTION("Minimizing") {
        OptTarget optTarget = MINIMIZE;

        OptSimulatedAnnealing opt(optBoundaries,
                                  maxCalculations,
                                  &myCalculator,
                                  optTarget,
                                  0.0, //only required if approaching / diverging
                                  coolingFactor,
                                  startChance);

        OptBase::run_optimisations();

        REQUIRE(fabs(opt.get_best_calculation().result - 0.0) < DELTA);
    }

    SECTION("Maximizing") {
        OptTarget optTarget = MAXIMIZE;

        OptSimulatedAnnealing opt(optBoundaries,
                                  maxCalculations,
                                  &myCalculator,
                                  optTarget,
                                  0.0, //only required if approaching / diverging
                                  coolingFactor,
                                  startChance);

        OptBase::run_optimisations();

        REQUIRE(fabs(opt.get_best_calculation().result - 25.0) < DELTA);
    }

    SECTION("Approaching") {
        OptTarget optTarget = APPROACH;

        OptSimulatedAnnealing opt(optBoundaries,
                                  maxCalculations,
                                  &myCalculator,
                                  optTarget,
                                  3.3, //only required if approaching / diverging
                                  coolingFactor,
                                  startChance);

        OptBase::run_optimisations();

        REQUIRE(fabs(opt.get_best_calculation().result - 3.3) < DELTA);
    }

    SECTION("Diverging1") {
        OptTarget optTarget = DIVERGE;

        OptSimulatedAnnealing opt(optBoundaries,
                                  maxCalculations,
                                  &myCalculator,
                                  optTarget,
                                  -100.0, //only required if approaching / diverging
                                  coolingFactor,
                                  startChance);

        OptBase::run_optimisations();

        REQUIRE(fabs(opt.get_best_calculation().result - 25.0) < DELTA);
    }

    SECTION("Diverging2") {
        OptTarget optTarget = DIVERGE;

        OptSimulatedAnnealing opt(optBoundaries,
                                  maxCalculations,
                                  &myCalculator,
                                  optTarget,
                                  100.0, //only required if approaching / diverging
                                  coolingFactor,
                                  startChance);

        OptBase::run_optimisations();

        REQUIRE(fabs(opt.get_best_calculation().result - 0.0) < DELTA);
    }
}

TEST_CASE("Threshold Accepting") {
    class MySolver : public SolverBase
    {
    public:
        void calculate(OptCalculation &optCalculation) const
        {
            optCalculation.result = pow(optCalculation.get_parameter("X"),2);
        }
    };

    OptBoundaries optBoundaries;
    optBoundaries.add_boundary(-5.0, 5.0, "X");

    MySolver myCalculator;
    unsigned int maxCalculations = 300;
    OPT_T coolingFactor = 0.95;
    OPT_T threshold = 5.0;
    OPT_T thresholdFactor = 0.95;

    SECTION("Minimizing") {
        OptTarget optTarget = MINIMIZE;

        OptThresholdAccepting opt(optBoundaries,
                                  maxCalculations,
                                  &myCalculator,
                                  optTarget,
                                  0.0, //only required if approaching / diverging
                                  coolingFactor,
                                  threshold,
                                  thresholdFactor);

        OptBase::run_optimisations();

        REQUIRE(fabs(opt.get_best_calculation().result - 0.0) < DELTA);
    }

    SECTION("Maximizing") {
        OptTarget optTarget = MAXIMIZE;

        OptThresholdAccepting opt(optBoundaries,
                                  maxCalculations,
                                  &myCalculator,
                                  optTarget,
                                  0.0, //only required if approaching / diverging
                                  coolingFactor,
                                  threshold,
                                  thresholdFactor);

        OptBase::run_optimisations();

        REQUIRE(fabs(opt.get_best_calculation().result - 25.0) < DELTA);
    }

    SECTION("Approaching") {
        OptTarget optTarget = APPROACH;

        OptThresholdAccepting opt(optBoundaries,
                                  maxCalculations,
                                  &myCalculator,
                                  optTarget,
                                  3.3, //only required if approaching / diverging
                                  coolingFactor,
                                  threshold,
                                  thresholdFactor);

        OptBase::run_optimisations();

        REQUIRE(fabs(opt.get_best_calculation().result - 3.3) < DELTA);
    }

    SECTION("Diverging1") {
        OptTarget optTarget = DIVERGE;

        OptThresholdAccepting opt(optBoundaries,
                                  maxCalculations,
                                  &myCalculator,
                                  optTarget,
                                  -100.0, //only required if approaching / diverging
                                  coolingFactor,
                                  threshold,
                                  thresholdFactor);

        OptBase::run_optimisations();

        REQUIRE(fabs(opt.get_best_calculation().result - 25.0) < DELTA);
    }

    SECTION("Diverging2") {
        OptTarget optTarget = DIVERGE;

        OptThresholdAccepting opt(optBoundaries,
                                  maxCalculations,
                                  &myCalculator,
                                  optTarget,
                                  100.0, //only required if approaching / diverging
                                  coolingFactor,
                                  threshold,
                                  thresholdFactor);

        OptBase::run_optimisations();

        REQUIRE(fabs(opt.get_best_calculation().result - 0.0) < DELTA);
    }
}

TEST_CASE("Multithreading / Boundary Splitting") {
    class MySolver : public SolverBase
    {
    public:
        void calculate(OptCalculation &optCalculation) const
        {
            optCalculation.result = pow(optCalculation.get_parameter("X"),2);
        }
    };

    OptBoundaries
            optBoundaries1,
            optBoundaries2,
            optBoundaries3,
            optBoundaries4;

    optBoundaries1.add_boundary(-5.0, -4.0, "X");
    optBoundaries2.add_boundary(-3.0, -1.0, "X");
    optBoundaries3.add_boundary(-0.5, 0.5, "X");
    optBoundaries4.add_boundary(5.0, 50.0, "X");

    MySolver myCalculator;
    unsigned int maxCalculations = 300;
    OPT_T coolingFactor = 0.95;
    OPT_T startChance = 0.25;

    SECTION("Minimizing") {
        OptTarget optTarget = MINIMIZE;

        OptSimulatedAnnealing opt1(optBoundaries1,
                                  maxCalculations,
                                  &myCalculator,
                                  optTarget,
                                  0.0, //only required if approaching / diverging
                                  coolingFactor,
                                  startChance);

        OptSimulatedAnnealing opt2(optBoundaries2,
                                  maxCalculations,
                                  &myCalculator,
                                  optTarget,
                                  0.0, //only required if approaching / diverging
                                  coolingFactor,
                                  startChance);

        OptSimulatedAnnealing opt3(optBoundaries3,
                                  maxCalculations,
                                  &myCalculator,
                                  optTarget,
                                  0.0, //only required if approaching / diverging
                                  coolingFactor,
                                  startChance);

        OptSimulatedAnnealing opt4(optBoundaries4,
                                  maxCalculations,
                                  &myCalculator,
                                  optTarget,
                                  0.0, //only required if approaching / diverging
                                  coolingFactor,
                                  startChance);

        OptBase::run_optimisations();

        REQUIRE(fabs(opt1.get_best_calculation().result - 16.0) < DELTA);
        REQUIRE(fabs(opt2.get_best_calculation().result - 1.0) < DELTA);
        REQUIRE(fabs(opt3.get_best_calculation().result - 0.0) < DELTA);
        REQUIRE(fabs(opt4.get_best_calculation().result - 25.0) < DELTA);

        REQUIRE(fabs(OptBase::get_best_calculation(optTarget, 0.0).result - 0.0) < DELTA);
    }

    SECTION("Maximizing") {
        OptTarget optTarget = MAXIMIZE;

        OptSimulatedAnnealing opt1(optBoundaries1,
                                  maxCalculations,
                                  &myCalculator,
                                  optTarget,
                                  0.0, //only required if approaching / diverging
                                  coolingFactor,
                                  startChance);

        OptSimulatedAnnealing opt2(optBoundaries2,
                                  maxCalculations,
                                  &myCalculator,
                                  optTarget,
                                  0.0, //only required if approaching / diverging
                                  coolingFactor,
                                  startChance);

        OptSimulatedAnnealing opt3(optBoundaries3,
                                  maxCalculations,
                                  &myCalculator,
                                  optTarget,
                                  0.0, //only required if approaching / diverging
                                  coolingFactor,
                                  startChance);

        OptSimulatedAnnealing opt4(optBoundaries4,
                                  maxCalculations,
                                  &myCalculator,
                                  optTarget,
                                  0.0, //only required if approaching / diverging
                                  coolingFactor,
                                  startChance);

        OptBase::run_optimisations();

        REQUIRE(fabs(opt1.get_best_calculation().result - 25.0) < DELTA);
        REQUIRE(fabs(opt2.get_best_calculation().result - 9.0) < DELTA);
        REQUIRE(fabs(opt3.get_best_calculation().result - 0.25) < DELTA);
        REQUIRE(fabs(opt4.get_best_calculation().result - 2500.0) < DELTA);

        REQUIRE(fabs(OptBase::get_best_calculation(optTarget, 0.0).result - 2500.0) < DELTA);
    }
}
