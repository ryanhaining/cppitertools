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

TEST_CASE("permutations: empty sequence has one empy permutation",
        "[permutations]") {
    const std::vector<int> ns{};
    auto p = permutations(ns);
    auto it = std::begin(p);
    REQUIRE( (*it).empty() );
    it++;
    REQUIRE( it == std::end(p) );
}

TEST_CASE("permutations: binds to lvalues, moves rvalues", "[permutations]") {
    itertest::BasicIterable<int> bi{1, 2};
    SECTION("binds to lvalues") {
        permutations(bi);
        REQUIRE_FALSE(bi.was_moved_from());
    }

    SECTION("moves rvalues") {
        permutations(std::move(bi));
        REQUIRE(bi.was_moved_from());
    }
}
