#include "helpers.hpp"
#include <utility>

#include "catch.hpp"

using itertest::SolidInt;

TEST_CASE("SolidInt can be moved only once", "[helpers]") {
    SolidInt i{3};
    SECTION("Doesn't throw on first move") {
        REQUIRE_NOTHROW( SolidInt{std::move(i)} );
    }
    SECTION("Throws on second move") {
        SolidInt i2{std::move(i)};
        REQUIRE_THROWS( SolidInt i3{std::move(i2)} );
    }
}
