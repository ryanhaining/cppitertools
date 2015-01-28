#include <slice.hpp>

#include <vector>
#include <string>
#include <utility>

#include "helpers.hpp"
#include "catch.hpp"

using iter::slice;
using Vec = const std::vector<int>;

TEST_CASE("slice: take from beginning", "[slice]") {
    Vec ns = {10,11,12,13,14,15,16,17,18,19};
    auto sl = slice(ns, 5);

    Vec v(std::begin(sl), std::end(sl));
    Vec vc = {10,11,12,13,14};

    REQUIRE( v == vc );
}

TEST_CASE("slice: start and stop", "[slice]") {
    Vec ns = {10,11,12,13,14,15,16,17,18,19};
    auto sl = slice(ns, 2, 6);

    Vec v(std::begin(sl), std::end(sl));
    Vec vc = {12, 13, 14, 15};

    REQUIRE( v == vc );
}

TEST_CASE("slice: start, stop, step", "[slice]") {
    Vec ns = {10,11,12,13,14,15,16,17,18,19};
    auto sl = slice(ns, 2, 8, 2);

    Vec v(std::begin(sl), std::end(sl));
    Vec vc = {12,14,16};

    REQUIRE( v == vc );
}

TEST_CASE("slice: stop is beyond end of iterable", "[slice]") {
    Vec ns = {1, 2, 3};
    auto sl = slice(ns, 10);

    Vec v(std::begin(sl), std::end(sl));
    REQUIRE( v == ns );
}

TEST_CASE("slice: start is beyond end of iterable", "[slice]") {
    Vec ns = {1, 2, 3};
    auto sl = slice(ns, 5, 10);
    REQUIRE( std::begin(sl) == std::end(sl) );
}

TEST_CASE("slice: (stop - start) % step != 0", "[slice]") {
    Vec ns = {1, 2, 3, 4};
    auto sl = slice(ns, 0, 2, 3);
    Vec v(std::begin(sl), std::end(sl));
    Vec vc = {1}; 

    REQUIRE( v == vc );
}

TEST_CASE("slice: invalid ranges give 0 size slices", "[slice]") {
    Vec ns = {1, 2, 3};
    SECTION("negative step") {
        auto sl = slice(ns, 1, 10, -1);
        REQUIRE( std::begin(sl) == std::end(sl) );
    }
    SECTION("stop < start") {
        auto sl = slice(ns, 2, 0, 3);
        REQUIRE( std::begin(sl) == std::end(sl) );
    }
}

TEST_CASE("slice: moves rvalues and binds to lvalues", "[slice]") {
    itertest::BasicIterable<int> bi{1, 2, 3, 4};
    slice(bi, 1, 3);
    REQUIRE_FALSE( bi.was_moved_from() );
    auto sl = slice(std::move(bi), 1, 3);
    REQUIRE( bi.was_moved_from() );

    Vec v(std::begin(sl), std::end(sl));
    Vec vc = {2, 3};

    REQUIRE( v == vc );
}


TEST_CASE("slice: with iterable doesn't move or copy elems", "[slice]") {
    constexpr std::array<itertest::SolidInt, 3> arr{{{6}, {7}, {8}}};
    for (auto&& i : slice(arr, 2)) {
        (void)i;
    }
}
