#include <unique_justseen.hpp>

#include "helpers.hpp"

#include <vector>
#include <string>
#include <iterator>

#include "catch.hpp"

using iter::unique_justseen;

using Vec = const std::vector<int>;

TEST_CASE("unique justseen: adjacent repeating values", "[unique_justseen]") {
    Vec ns = {1,1,1,2,2,3,4,4,5,6,7,8,8,8,8,9,9};
    auto ue = unique_justseen(ns);
    Vec v(std::begin(ue), std::end(ue));
    Vec vc = {1,2,3,4,5,6,7,8,9};
    REQUIRE( v == vc );
}
