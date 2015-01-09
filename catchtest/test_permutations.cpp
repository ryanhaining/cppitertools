#include <permutations.hpp>

#include "helpers.hpp"

#include <vector>
#include <string>
#include <iterator>

#include "catch.hpp"

using iter::permutations;
using IntPermSet = std::multiset<std::vector<int>>;

TEST_CASE("permutations: basic test, 3 element sequence", "[permutations]") {
    const std::vector<int> ns = {1, 7, 9};
    auto p = permutations(ns);

    IntPermSet v;
    for (auto&& st : p) {
        v.emplace(std::begin(st), std::end(st));
    }

    const IntPermSet vc =
        {{1, 7, 9}, {1, 9, 7}, {7, 1, 9}, {7, 9, 1}, {9, 1, 7}, {9, 7, 1}};
    REQUIRE( v == vc );
}
