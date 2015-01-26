#include <unique_everseen.hpp>

#include "helpers.hpp"

#include <vector>
#include <string>
#include <iterator>

#include "catch.hpp"

using iter::unique_everseen;

using Vec = const std::vector<int>;

TEST_CASE("unique everseen: adjacent repeating values", "[unique_everseen]") {
    Vec ns = {1,1,1,2,2,3,4,4,5,6,7,8,8,8,8,9,9};
    auto ue = unique_everseen(ns);
    Vec v(std::begin(ue), std::end(ue));
    Vec vc = {1,2,3,4,5,6,7,8,9};
    REQUIRE( v == vc );
}

TEST_CASE("unique everseen: nonadjacent repeating values",
        "[unique_everseen]") {
    Vec ns = {1,2,3,4,3,2,1,5,6};
    auto ue = unique_everseen(ns);
    Vec v(std::begin(ue), std::end(ue));
    Vec vc = {1,2,3,4,5,6};
    REQUIRE( v == vc );
}
