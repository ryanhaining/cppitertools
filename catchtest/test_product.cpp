#include <product.hpp>

#include "helpers.hpp"

#include <vector>
#include <string>
#include <iterator>

#include "catch.hpp"

using iter::product;
using Vec = const std::vector<int>;

TEST_CASE("product: basic test, two vectors", "[product]") {
    using ResType = std::vector<std::tuple<int, int>>;
    using TP = std::tuple<int, int>;

    Vec n1 = {0, 1};
    Vec n2 = {0, 1, 2};

    auto p = product(n1, n2);
    ResType v(std::begin(p), std::end(p));
    ResType vc = {TP{0,0}, TP{0,1}, TP{0,2}, TP{1,0}, TP{1,1}, TP{1,2}};

    REQUIRE( v == vc );
}

TEST_CASE("product: empty when any iterable is empty", "[product]") {
    Vec n1 = {0, 1};
    Vec n2 = {0, 1, 2};
    Vec emp = {};

    SECTION("first iterable is empty") {
        auto p = product(emp, n1, n2);
        REQUIRE( std::begin(p) == std::end(p) );
    }

    SECTION("middle iterable is empty") {
        auto p = product(n1, emp, n2);
        REQUIRE( std::begin(p) == std::end(p) );
    }

    SECTION("last iterable is empty") {
        auto p = product(n1, n2, emp);
        REQUIRE( std::begin(p) == std::end(p) );
    }
}
