#include <sliding_window.hpp>

#include <vector>
#include <array>
#include <string>
#include <utility>

#include "helpers.hpp"
#include "catch.hpp"

using iter::sliding_window;
using Vec = const std::vector<int>;

TEST_CASE("sliding_window: window of size 3", "[sliding_window]") {
    Vec ns = { 10, 20, 30, 40, 50};
    auto sw = sliding_window(ns, 3);
    auto it = std::begin(sw);
    REQUIRE( it != std::end(sw) );
    {
        Vec v(std::begin(*it), std::end(*it));
        Vec vc = {10, 20, 30};
        REQUIRE( v == vc );

    }
    ++it;
    REQUIRE( it != std::end(sw) );
    {
        Vec v(std::begin(*it), std::end(*it));
        Vec vc = {20, 30, 40};
        REQUIRE( v == vc );
    }
    ++it;
    REQUIRE( it != std::end(sw) );
    {
        Vec v(std::begin(*it), std::end(*it));
        Vec vc = {30, 40, 50};
        REQUIRE( v == vc );
    }
    ++it;
    REQUIRE( it == std::end(sw) );
}

TEST_CASE("sliding window: oversized window is empty", "[sliding_window]") {
    Vec ns = {10, 20, 30};
    auto sw = sliding_window(ns, 5);
    REQUIRE( std::begin(sw) == std::end(sw) );
}

TEST_CASE("sliding window: window size == len(iterable)", "[sliding_window]") {
    Vec ns = {10, 20, 30};
    auto sw = sliding_window(ns, 3);
    auto it = std::begin(sw);
    REQUIRE( it != std::end(sw) );

    Vec v(std::begin(*it), std::end(*it));

    REQUIRE( ns == v );
    ++it;
    REQUIRE( it == std::end(sw) );
}

TEST_CASE("sliding window: empty iterable is empty", "[sliding_window]") {
    Vec ns{};
    auto sw = sliding_window(ns, 1);
    REQUIRE( std::begin(sw) == std::end(sw) );
}

TEST_CASE("sliding window: window size of 1", "[sliding_window]") {
    Vec ns = {10, 20, 30};
    auto sw = sliding_window(ns, 1);
    auto it = std::begin(sw);
    REQUIRE( *std::begin(*it) == 10 );
    ++it;
    REQUIRE( *std::begin(*it) == 20 );
    ++it;
    REQUIRE( *std::begin(*it) == 30 );
    ++it;
    REQUIRE( it == std::end(sw) );
}

TEST_CASE("sliding window: window size of 0", "[sliding_window]") {
    Vec ns = {10, 20, 30};
    auto sw = sliding_window(ns, 0);
    REQUIRE( std::begin(sw) == std::end(sw) );
}
