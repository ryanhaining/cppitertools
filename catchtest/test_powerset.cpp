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

TEST_CASE("powerset: empty sequence gives only empty set", "[powerset]") {
    const std::vector<int> ns = {};
    auto ps = powerset(ns);
    auto it = std::begin(ps);
    REQUIRE( std::begin(*it) == std::end(*it) ); // it's empty
    ++it;
    REQUIRE( it == std::end(ps) );
}

TEST_CASE("powerset: iterators can be compared", "[powerset]") {
    std::vector<int> ns = {1, 2};
    auto p = powerset(ns);
    {
        auto it = std::begin(p);
        REQUIRE( it == std::begin(p) );
        REQUIRE_FALSE( it != std::begin(p) );
        REQUIRE( it != std::end(p) );
        REQUIRE_FALSE( it == std::end(p) );
        ++it;
        REQUIRE_FALSE( it == std::begin(p) );
        REQUIRE( it != std::begin(p) );
        REQUIRE_FALSE( it == std::end(p) );
        REQUIRE( it != std::end(p) );
        ++it;
        ++it;
        ++it;
        REQUIRE( it == std::end(p) );
    }

    ns.push_back(3);
    {
        auto it = std::begin(p);
        auto it2 = std::begin(p);
        std::advance(it, 4);
        std::advance(it2, 4);
        REQUIRE( it == it2 );
        ++it2;
        REQUIRE( it != it2 );
    }

}

TEST_CASE("powerset: iterator copy ctor is correct", "[powerset]") {
    // { {}, {1}, {2}, {1, 2} }
    std::vector<int> ns = {1, 2};
    auto p = powerset(ns);
    auto it = std::begin(p);
    auto it2(it);
    REQUIRE( it == it2 );
    ++it2;
    REQUIRE( it != it2 );
    REQUIRE( std::begin(*it) == std::end(*it) );
}



TEST_CASE("powerset: binds to lvalues, moves rvalues", "[powerset]") {
    itertest::BasicIterable<int> bi{1, 2};
    SECTION("binds to lvalues") {
        powerset(bi);
        REQUIRE_FALSE(bi.was_moved_from());
    }
    SECTION("moves rvalues") {
        powerset(std::move(bi));
        REQUIRE(bi.was_moved_from());
    }
}

TEST_CASE("powerset: doesn't move or copy elements of iterable", "[powerset]"){
    constexpr itertest::SolidInt arr[] = {{1}, {0}, {2}};
    for (auto&& st : powerset(arr)) {
        for (auto&& i : st) {
            (void)i;
        }
    }
}
