#include <accumulate.hpp>

#include <vector>
#include <iterator>
#include <string>

#include "catch.hpp"

using iter::accumulate;

TEST_CASE("Simple sum", "[accumulate]") {
    std::vector<int> ns{1,2,3,4,5};
    auto a = accumulate(ns);

    const std::vector<int> v(std::begin(a), std::end(a));
    const std::vector<int> vc{1,3,6,10,15};
    REQUIRE( v == vc );
}

