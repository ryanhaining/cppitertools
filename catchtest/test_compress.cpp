#include "helpers.hpp"
#include <compress.hpp>

#include <vector>
#include <list>
#include <string>
#include <vector>
#include <iterator>
#include <utility>

#include "catch.hpp"

using iter::compress;
using itertest::SolidInt;
using itertest::BasicIterable;
using Vec = const std::vector<char>;

TEST_CASE("compress: alternating", "[compress]") {
    std::vector<int> ivec{1, 2, 3, 4, 5, 6};
    std::vector<bool> bvec{true, false, true, false, true, false};
    auto c = compress(ivec, bvec);
    Vec v(std::begin(c), std::end(c));
    Vec vc = {1,3,5};

    REQUIRE( v  == vc );
}

TEST_CASE("compress: consecutive falses", "[compress]") {
    std::vector<int> ivec{1, 2, 3, 4, 5};
    std::vector<bool> bvec{true, false, false, false, true};
    auto c = compress(ivec, bvec);
    Vec v(std::begin(c), std::end(c));
    Vec vc = {1,5};

    REQUIRE( v == vc );
}

TEST_CASE("compress: consecutive trues", "[compress]") {
    std::vector<int> ivec{1, 2, 3, 4, 5};
    std::vector<bool> bvec{false, true, true, true, false};
    auto c = compress(ivec, bvec);
    Vec v(std::begin(c), std::end(c));
    Vec vc = {2,3,4}

    REQUIRE( v == vc );
}
