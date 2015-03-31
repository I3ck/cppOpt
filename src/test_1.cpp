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
        void calculate(OptValue &optValue) const
        {
            optValue.result = pow(optValue.get_parameter("X"),2);
        }
    };

    OptBoundaries optBoundaries;
    optBoundaries.add_boundary(-5.0, 5.0, "X");

    MySolver myCalculator;
    unsigned int maxCalculations = 300;
    T coolingFactor = 0.95;
    T startChance = 0.25;

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

        REQUIRE(fabs(opt.get_best_calculation().result - 0.0) < 0.001);
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

        REQUIRE(fabs(opt.get_best_calculation().result - 25.0) < 0.001);
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

        REQUIRE(fabs(opt.get_best_calculation().result - 3.3) < 0.001);
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

        REQUIRE(fabs(opt.get_best_calculation().result - 25.0) < 0.001);
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

        REQUIRE(fabs(opt.get_best_calculation().result - 0.0) < 0.001);
    }
}

TEST_CASE("Threshold Accepting") {
    class MySolver : public SolverBase
    {
    public:
        void calculate(OptValue &optValue) const
        {
            optValue.result = pow(optValue.get_parameter("X"),2);
        }
    };

    OptBoundaries optBoundaries;
    optBoundaries.add_boundary(-5.0, 5.0, "X");

    MySolver myCalculator;
    unsigned int maxCalculations = 300;
    T coolingFactor = 0.95;
    T threshold = 5.0;
    T thresholdFactor = 0.95;

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

        REQUIRE(fabs(opt.get_best_calculation().result - 0.0) < 0.001);
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

        REQUIRE(fabs(opt.get_best_calculation().result - 25.0) < 0.001);
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

        REQUIRE(fabs(opt.get_best_calculation().result - 3.3) < 0.001);
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

        REQUIRE(fabs(opt.get_best_calculation().result - 25.0) < 0.001);
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

        REQUIRE(fabs(opt.get_best_calculation().result - 0.0) < 0.001);
    }
}
