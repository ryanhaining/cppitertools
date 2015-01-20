#include <reversed.hpp>

#include <vector>
#include <string>

#include "helpers.hpp"
#include "catch.hpp"

using iter::reversed;
using Vec = const std::vector<int>;

TEST_CASE("Reversing a vector", "[reversed]") {
    Vec ns = {10, 20, 30, 40};
    auto r = reversed(ns);

    Vec v(std::begin(r), std::end(r));
    Vec vc = {40, 30, 20, 10};

    REQUIRE( v == vc );
}
