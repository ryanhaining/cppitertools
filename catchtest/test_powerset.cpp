#include <powerset.hpp>

#include "helpers.hpp"

#include <vector>
#include <string>
#include <iterator>

#include "catch.hpp"

using iter::powerset;
using IntPermSet = std::multiset<std::multiset<int>>;

TEST_CASE("powerset: basic test, [1, 2, 3]", "[powerset]") {
    const std::vector<int> ns = {1, 2, 3};
    IntPermSet v;
    for (auto&& st : powerset(ns)) {
        v.emplace(std::begin(st), std::end(st));
    }

    const IntPermSet vc = { {}, {1}, {2}, {3,}, {1,2}, {1,3}, {2,3}, {1,2,3} };
    REQUIRE( v == vc );
}
