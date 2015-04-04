#include <sorted.hpp>

#include <set>
#include <unordered_set>
#include <vector>
#include <array>
#include <string>
#include <utility>

#include "helpers.hpp"
#include "catch.hpp"

using iter::sorted;

using Vec = const std::vector<int>;

TEST_CASE("sorted: iterates through a vector in sorted order", "[sorted]" ){
    Vec ns = {4, 0, 5, 1, 6, 7, 9, 3, 2, 8};
    auto s = sorted(ns);
    Vec v(std::begin(s), std::end(s));
    Vec vc = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    REQUIRE( v == vc );
}
