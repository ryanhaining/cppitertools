#include <product.hpp>

#include "helpers.hpp"

#include <vector>
#include <string>
#include <iterator>

#include "catch.hpp"

using iter::product;
using Vec = const std::vector<int>;

TEST_CASE("product: basic test, two sequences", "[product]") {
    using TP = std::tuple<int, char>;
    using ResType = std::vector<TP>;

    Vec n1 = {0, 1};
    const std::string s{"abc"};

    auto p = product(n1, s);
    ResType v(std::begin(p), std::end(p));
    ResType vc = {TP{0,'a'}, TP{0,'b'}, TP{0,'c'},
                  TP{1,'a'}, TP{1,'b'}, TP{1,'c'}};

    REQUIRE( v == vc );
}

TEST_CASE("product: three sequences", "[product]") {
    using TP = std::tuple <int, char, int>;
    using ResType = const std::vector<TP>;

    Vec n1 = {0, 1};
    const std::string s{"ab"};
    Vec n2 = {2};

    auto p = product(n1, s, n2);
    ResType v(std::begin(p), std::end(p));

    ResType vc = {
        TP{0, 'a', 2},
        TP{0, 'b', 2},
        TP{1, 'a', 2},
        TP{1, 'b', 2}
    };

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

TEST_CASE("product: single iterable", "[product]") {
    const std::string s{"ab"};
    using TP = std::tuple<char>;
    using ResType = const std::vector<TP>;

    auto p = product(s);
    ResType v(std::begin(p), std::end(p));
    ResType vc = {TP{'a'}, TP{'b'}};

    REQUIRE( v == vc );
}
