#include "helpers.hpp"
#include <count.hpp>

#include <vector>
#include <iterator>
#include <utility>

#include "catch.hpp"

using iter::count;

TEST_CASE("count: watch for 10 elements", "[count]") {
    std::vector<int> v{};
    for (auto i : count()) {
        v.push_back(i);
        if (i == 9) break;
    }

    const std::vector<int> vc{0,1,2,3,4,5,6,7,8,9};
    REQUIRE( v == vc );
}

TEST_CASE("count: start at 10", "[count]") {
    std::vector<int> v{};
    for (auto i : count(10)) {
        v.push_back(i);
        if (i == 14) break;
    }

    const std::vector<int> vc{10,11,12,13,14};
    REQUIRE( v == vc );
}

TEST_CASE("count with step", "[count]") {
    std::vector<int> v{};
    for (auto i : count(2, -1)) {
        v.push_back(i);
        if (i == -3) break;
    }

    const std::vector<int> vc{2,1,0,-1,-2,-3};
    REQUIRE( v == vc);
}
