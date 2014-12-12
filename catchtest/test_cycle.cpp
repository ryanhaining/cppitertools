#include <cycle.hpp>

#include "helpers.hpp"

#include <vector>
#include <string>
#include <iterator>

#include "catch.hpp"

using iter::cycle;

TEST_CASE("cycle: iterate twice", "[cycle]") {
    std::vector<int> ns {2,4,6};
    std::vector<int> v{};
    std::size_t count = 0;
    for (auto i : cycle(ns)) {
        v.push_back(i);
        ++count;
        if (count == ns.size() * 2) break;
    }

    auto vc = ns;
    vc.insert(std::end(vc), std::begin(ns), std::end(ns));
    REQUIRE( v == vc );
}
