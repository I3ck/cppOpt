#define CATCH_CONFIG_MAIN
#include "../dependencies/Catch.h" //https://github.com/philsquared/Catch

#include "OptBoundary.h"
#include "OptBoundaries.h"

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
