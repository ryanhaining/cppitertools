#include <zip.hpp>

#include "test_helpers.hpp"

#include <vector>
#include <tuple>
#include <string>
#include <iterator>
#include <utility>
#include <sstream>
#include <iterator>

#include "catch.hpp"

using iter::zip;
using itertest::BasicIterable;
using itertest::SolidInt;

TEST_CASE("Simple case, same length", "[zip]") {
    using Tu = std::tuple<int, char, double>;
    using ResVec = const std::vector<Tu>;
    std::vector<int> iv {10, 20, 30};
    std::string s{"hey"};
    double arr[] = {1.0, 2.0, 4.0};

    auto z = zip(iv, s, arr);
    ResVec v(std::begin(z), std::end(z));
    ResVec vc {Tu{10, 'h', 1.0}, Tu{20, 'e', 2.0}, Tu{30, 'y', 4.0}};
    REQUIRE( v == vc );
}

TEST_CASE("One empty, all empty", "[zip]") {
    std::vector<int> iv = {1,2,3};
    std::string s{};
    auto z = zip(iv, s);
    REQUIRE_FALSE( std::begin(z) != std::end(z) );
    auto z2 = zip(s, iv);
    REQUIRE_FALSE( std::begin(z2) != std::end(z2) );
}

TEST_CASE("terminates on shortest sequence", "[zip]") {
    std::vector<int> iv{1,2,3,4,5};
    std::string s{"hi"};
    auto z = zip(iv, s);

    REQUIRE( std::distance(std::begin(z), std::end(z)) == 2 );
}

TEST_CASE("Empty zip()", "[zip]") {
    auto z = zip();
    REQUIRE_FALSE( std::begin(z) != std::end(z) );
}

TEST_CASE("Modify sequence through zip", "[zip]") {
    std::vector<int> iv{1,2,3};
    for (auto&& t : zip(iv)) {
        std::get<0>(t) = -1;
    }

    const std::vector<int> vc{-1, -1, -1};
    REQUIRE( iv == vc);
}

TEST_CASE("Binds reference when it should", "[zip]") {
    BasicIterable<char> bi{'x', 'y', 'z'};
    zip(bi);
    REQUIRE_FALSE( bi.was_moved_from() );
}

TEST_CASE("Moves rvalues", "[zip]") {
    BasicIterable<char> bi{'x', 'y', 'z'};
    zip(std::move(bi));
    REQUIRE( bi.was_moved_from() );
}

TEST_CASE("Can bind ref and move in single zip", "[zip]") {
    BasicIterable<char> b1{'x', 'y', 'z'};
    BasicIterable<char> b2{'a', 'b'};
    zip(b1, std::move(b2));
    REQUIRE_FALSE( b1.was_moved_from() );
    REQUIRE( b2.was_moved_from() );
}
