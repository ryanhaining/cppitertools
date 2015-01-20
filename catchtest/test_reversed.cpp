#include <reversed.hpp>

#include <vector>
#include <string>
#include <utility>

#include "helpers.hpp"
#include "catch.hpp"

using iter::reversed;
using Vec = const std::vector<int>;

TEST_CASE("reversed: can reverse a vector", "[reversed]") {
    Vec ns = {10, 20, 30, 40};
    auto r = reversed(ns);

    Vec v(std::begin(r), std::end(r));
    Vec vc = {40, 30, 20, 10};

    REQUIRE( v == vc );
}

TEST_CASE("reversed: can reverse an array", "[reversed]") {
    int ns[] = {10, 20, 30, 40};
    auto r = reversed(ns);

    Vec v(std::begin(r), std::end(r));
    Vec vc = {40, 30, 20, 10};

    REQUIRE( v == vc );
}

TEST_CASE("reversed: empty when iterable is empty", "[reversed]") {
    Vec emp{};
    auto r = reversed(emp);
    REQUIRE( std::begin(r) == std::end(r) );
}

TEST_CASE("reversed: moves rvalues and binds to lvalues", "[reversed]") {
    itertest::BasicIterable<int> bi{1, 2};
    itertest::BasicIterable<int> bi2{1, 2};
    reversed(bi);
    REQUIRE_FALSE( bi.was_moved_from() );

    reversed(std::move(bi2));
    REQUIRE( bi2.was_moved_from() );
}
