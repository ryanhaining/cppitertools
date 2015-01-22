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
    REQUIRE( !(it != std::end(sw)) );
}
