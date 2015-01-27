#include <slice.hpp>

#include <vector>
#include <string>
#include <utility>

#include "helpers.hpp"
#include "catch.hpp"

using iter::slice;
using Vec = const std::vector<int>;

TEST_CASE("slice: take from beginning", "[slice]") {
    Vec ns = {10,11,12,13,14,15,16,17,18,19};
    auto sl = slice(ns, 5);

    Vec v(std::begin(sl), std::end(sl));
    Vec vc = {10,11,12,13,14};

    REQUIRE( v == vc );
}

TEST_CASE("slice: start and stop", "[slice]") {
    Vec ns = {10,11,12,13,14,15,16,17,18,19};
    auto sl = slice(ns, 2, 6);

    Vec v(std::begin(sl), std::end(sl));
    Vec vc = {12, 13, 14, 15};

    REQUIRE( v == vc );
}
