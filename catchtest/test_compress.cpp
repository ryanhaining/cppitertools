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
using Vec = const std::vector<int>;

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
    Vec vc = {2,3,4};

    REQUIRE( v == vc );
}

TEST_CASE("compress: all true", "[compress]") {
    std::vector<int> ivec{1, 2, 3, 4, 5};
    std::vector<bool> bvec(ivec.size(), true);
    auto c = compress(ivec, bvec);
    Vec v(std::begin(c), std::end(c));

    REQUIRE( v == ivec );
}

TEST_CASE("compress: all false", "[compress]") {
    std::vector<int> ivec{1, 2, 3, 4, 5};
    std::vector<bool> bvec(ivec.size(), false);
    auto c = compress(ivec, bvec);
    REQUIRE( std::begin(c) == std::end(c) );
}

TEST_CASE("compress: binds to lvalues, moves rvalues", "[compress]") {
    BasicIterable<char> bi{'x', 'y', 'z'};
    std::vector<bool> bl{true, true, true};
    SECTION("binds to lvalues") {
        compress(bi, bl);
        REQUIRE_FALSE( bi.was_moved_from() );
    }
    SECTION("moves rvalues") {
        compress(std::move(bi), bl);
        REQUIRE( bi.was_moved_from() );
    }
}

struct BoolLike {
    public:
        bool state;
        explicit operator bool() const {
            return this->state;
        }
};

TEST_CASE("compress: workds with truthy and falsey values", "[compress]") {
    std::vector<BoolLike> bvec{{true}, {false}, {true}, {false}};

    Vec ivec{1,2,3,4};

    auto c = compress(ivec, bvec);
    Vec v(std::begin(c), std::end(c));

    Vec vc = {1,3};
    REQUIRE( v == vc );
}
