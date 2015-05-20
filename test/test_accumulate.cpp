#include <accumulate.hpp>
#include "helpers.hpp"

#include <vector>
#include <iterator>
#include <string>

#include "catch.hpp"

using iter::accumulate;
using itertest::BasicIterable;

using Vec = const std::vector<int>;
TEST_CASE("Simple sum", "[accumulate]") {
    Vec ns{1,2,3,4,5};
    auto a = accumulate(ns);

    Vec v(std::begin(a), std::end(a));
    Vec vc{1,3,6,10,15};
    REQUIRE( v == vc );
}

TEST_CASE("accumulate: With subtraction lambda", "[accumulate]") {
    Vec ns{5,4,3,2,1};
    auto a = accumulate(ns, [](int a, int b){return a - b; });

    Vec v(std::begin(a), std::end(a));
    Vec vc{5, 1, -2, -4, -5};
    REQUIRE( v == vc );
}

TEST_CASE("accumulate: with initializer_list works", "[accumulate]") {
    auto a = accumulate({1, 2, 3});
    Vec v(std::begin(a), std::end(a));
    Vec vc{1, 3, 6};

    REQUIRE( v == vc );
}

TEST_CASE("accumulate: binds reference when it should", "[accumulate]") {
    BasicIterable<int> bi{1, 2};
    accumulate(bi);
    REQUIRE_FALSE( bi.was_moved_from() );
}

TEST_CASE("accumulate: moves rvalues when it should", "[accumulate]") {
    BasicIterable<int> bi{1, 2};
    accumulate(std::move(bi));
    REQUIRE( bi.was_moved_from() );
}

TEST_CASE("accumulate: operator==", "[accumulate]") {
    Vec v;
    auto a = accumulate(v);
    REQUIRE( std::begin(a) == std::end(a) );
}

TEST_CASE("accumulate: postfix ++", "[accumulate]") {
    Vec ns{2,3};
    auto a = accumulate(ns);
    auto it = std::begin(a);
    it++;
    REQUIRE( *it == 5 );
}


TEST_CASE("accumulate: iterator meets requirements", "[accumulate]") {
    Vec ns{};
    auto a = accumulate(ns);
    REQUIRE( itertest::IsIterator<decltype(std::begin(a))>::value );
}
