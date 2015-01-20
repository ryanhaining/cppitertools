#include <repeat.hpp>

#include "helpers.hpp"

#include <vector>
#include <string>
#include <iterator>

#include "catch.hpp"

using iter::repeat;

TEST_CASE("repeat: one argument keeps giving value back", "[repeat]") {
    auto r = repeat('a');
    auto it = std::begin(r);
    REQUIRE( *it == 'a' );
    ++it;
    REQUIRE( *it == 'a' );
    ++it;
    REQUIRE( *it == 'a' );
    ++it;
    REQUIRE( *it == 'a' );
    ++it;
    REQUIRE( *it == 'a' );
}
