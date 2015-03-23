#define CATCH_CONFIG_MAIN
#include "../dependencies/Catch.h" //https://github.com/philsquared/Catch

using namespace std;


TEST_CASE("first case") {

    SECTION("first section") {
        REQUIRE(true);
    }
}
