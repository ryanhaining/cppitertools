#include "range.hpp"

#include <vector>
#include <iterator>
#include <algorithm>

#include "helpers.hpp"
#include "catch.hpp"

using Vec = const std::vector<int>;
using iter::range;


TEST_CASE("range: works with only stop", "[range]") {
    auto r = range(5);
    Vec v(std::begin(r), std::end(r));
    Vec vc{0, 1, 2, 3, 4};

    REQUIRE( v == vc );
}

TEST_CASE("range: works with start and stop", "[range]") {
    auto r = range(1, 5);
    Vec v(std::begin(r), std::end(r));
    Vec vc {1, 2, 3, 4};

    REQUIRE( v == vc );
}

TEST_CASE("range: works with positive step > 1", "[range]") {
    auto r = range(1, 10, 3);
    Vec v(std::begin(r), std::end(r));
    Vec vc{1, 4, 7};

    REQUIRE( v == vc );
}

TEST_CASE("range(0) is empty", "[range]") {
    auto r = iter::range(0);
    REQUIRE( std::begin(r) == std::end(r) );
}

TEST_CASE("range: postfix++", "[range]") {
    auto r = iter::range(3);
    auto it = std::begin(r);
    it++;
    REQUIRE( *it == 1 );
}

TEST_CASE("start > stop produces empty range", "[range]") {
    auto r = range(5, 0);
    Vec v(std::begin(r), std::end(r));
    REQUIRE( v.empty() );
}

TEST_CASE("start < stop and step < 0 produces empty range", "[range]") {
    auto r = range(0, 5, -1);
    Vec v(std::begin(r), std::end(r));
    REQUIRE( v.empty() );
}

TEST_CASE("range: with only a negative stop is empty", "[range]") {
    auto r = range(-3);
    Vec v(std::begin(r), std::end(r));

    REQUIRE( v.empty() );
}

TEST_CASE("range: works with negative step", "[range]") {
    auto r = range(5, -5, -3);
    Vec v(std::begin(r), std::end(r));
    Vec vc{5, 2, -1, -4};

    REQUIRE( v == vc );
}

TEST_CASE("range: stops short when step doesn't divide stop-start", "[range]") {
    auto r = range(0, 5, 2);
    Vec v(std::begin(r), std::end(r));
    Vec vc{0, 2, 4};

    REQUIRE( v == vc );

}

TEST_CASE("range: stops short when step > stop-start", "[range]") {
    auto r = range(0, 10, 20);
    Vec v(std::begin(r), std::end(r));
    REQUIRE( v.size() == 1 );
}

TEST_CASE("No 0 step ranges allowed", "[range]") {
    REQUIRE_THROWS(range(0, 1, 0));
}

TEST_CASE("range: works with a variable start, stop, and step", "[range]") {
    constexpr int a = 10;
    constexpr int b = 100;
    constexpr int c = 50;
    SECTION("Going up works") {
        auto r = range(a, a+2);
        Vec v(std::begin(r), std::end(r));
        Vec vc{a, a+1};
        REQUIRE( v == vc );
    }

    SECTION("Going down works") {
        auto r = range(a+2, a, -1);
        Vec v(std::begin(r), std::end(r));
        Vec vc{a+2, a+1};
        REQUIRE( v == vc );
    }

    SECTION("Going down with -2 stop works") {
        auto r = range(a+4, a, -2);
        Vec v(std::begin(r), std::end(r));
        Vec vc{a+4, a+2};
        REQUIRE( v == vc );
    }

    SECTION("Using three variable") {
        auto r = range(a, b, c);
        Vec v(std::begin(r), std::end(r));
        REQUIRE( std::find(std::begin(v), std::end(v), a) != std::end(v) );
        REQUIRE( std::find(std::begin(v), std::end(v), b) == std::end(v) );
        REQUIRE( v.size() == 2 );
    }

    SECTION("Using three with a unary negate on step") {
        auto r = range(b, a, -c);
        Vec v(std::begin(r), std::end(r));

        REQUIRE( std::find(std::begin(v), std::end(v), b) != std::end(v) );
        REQUIRE( std::find(std::begin(v), std::end(v), a) == std::end(v) );
        REQUIRE( v.size() == 2 );
    }

    SECTION("Using all three negated") {
        auto r = range(-a, -b, -c);
        Vec v(std::begin(r), std::end(r));

        REQUIRE( std::find(std::begin(v), std::end(v), -a) != std::end(v) );
        REQUIRE( std::find(std::begin(v), std::end(v), -b) == std::end(v) );
        REQUIRE( v.size() == 2 );
    }

}

using FVec = const std::vector<double>;

TEST_CASE("range: using doubles", "[range]") {
    auto r = range(5.0);
    FVec fv(std::begin(r), std::end(r));
    FVec fvc = {0.0, 1.0, 2.0, 3.0, 4.0};
    REQUIRE( fv == fvc );
}

TEST_CASE("range: using doubles with start and stop", "[range]") {
    auto r = range(5.0, 10.0);
    FVec fv(std::begin(r), std::end(r));
    FVec fvc = {5.0, 6.0, 7.0, 8.0, 9.0};
    REQUIRE( fv == fvc );
}

TEST_CASE("range: using doubles with start, stop and step", "[range]") {
    auto r = range(1.0, 4.0, 0.5);
    FVec fv(std::begin(r), std::end(r));
    FVec fvc = {1.0, 1.5, 2.0, 2.5, 3.0, 3.5};
    REQUIRE( fv == fvc );
}

TEST_CASE("range: using doubles with negative", "[range]") {
    auto r = range(0.5, -2.0, -0.5);
    FVec fv(std::begin(r), std::end(r));
    FVec fvc = {0.5, 0.0, -0.5, -1.0, -1.5};
    REQUIRE( fv == fvc );
}

TEST_CASE("range: using doubles with uneven step", "[range]") {
    auto r = range(0.0, 1.75, 0.5);
    FVec fv(std::begin(r), std::end(r));
    FVec fvc = {0.0, 0.5, 1.0, 1.5};
    REQUIRE( fv == fvc );
}

TEST_CASE("range: using doubles detects empty ranges", "[range]") {
    auto r1 = range(0.0, -1.0);
    REQUIRE(std::begin(r1) == std::end(r1));

    auto r2 = range(0.0, 1.0, -1.0);
    REQUIRE(std::begin(r2) == std::end(r2));
}

TEST_CASE("range: iterator meets forward iterator requirements", "[range]") {
    auto r = range(5);
    auto r2 = range(5.0);
    REQUIRE( itertest::IsForwardIterator<decltype(std::begin(r))>::value );
    REQUIRE( itertest::IsForwardIterator<decltype(std::begin(r2))>::value );
}
