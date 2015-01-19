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

