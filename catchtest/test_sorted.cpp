#include <sorted.hpp>

#include <set>
#include <unordered_set>
#include <vector>
#include <array>
#include <string>
#include <utility>

#include "helpers.hpp"
#include "catch.hpp"

using iter::sorted;

using Vec = const std::vector<int>;

TEST_CASE("sorted: iterates through a vector in sorted order", "[sorted]" ){
    Vec ns = {4, 0, 5, 1, 6, 7, 9, 3, 2, 8};
    auto s = sorted(ns);
    Vec v(std::begin(s), std::end(s));
    Vec vc = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    REQUIRE( v == vc );
}

TEST_CASE("sorted: can modify elements through sorted", "[sorted]") {
    std::vector<int> ns(3, 9);
    for (auto&& n : sorted(ns)) {
        n = -1;
    }
    Vec vc(3, -1);
    REQUIRE( ns == vc );
}

TEST_CASE("sorted: can iterate over unordered container", "[sorted]") {
    std::unordered_set<int> ns = {1, 3, 2, 0, 4};
    auto s = sorted(ns);

    Vec v(std::begin(s), std::end(s));
    Vec vc = {0, 1, 2, 3, 4};
    REQUIRE( v == vc );
}

TEST_CASE("sorted: empty when iterable is empty", "[sorted]") {
    Vec ns{};
    auto s = sorted(ns);
    REQUIRE( std::begin(s) == std::end(s) );
}

namespace {
    bool int_greater_than(int lhs, int rhs) {
        return lhs > rhs;
    }

    struct IntGreaterThan {
        bool operator() (int lhs, int rhs) const {
            return lhs > rhs;
        }
    };
}

TEST_CASE("sorted: works with different functor types", "[sorted]") {
    Vec ns = {4, 1, 3, 2, 0};
    std::vector<int> v;
    SECTION("with function pointer") {
        auto s = sorted(ns, int_greater_than);
        v.insert(v.begin(), std::begin(s), std::end(s));
    }

    SECTION("with callable object") {
        auto s = sorted(ns, IntGreaterThan{});
        v.insert(v.begin(), std::begin(s), std::end(s));
    }

    SECTION("with lambda") {
        auto s = sorted(ns, [](int lhs, int rhs){return lhs > rhs;});
        v.insert(v.begin(), std::begin(s), std::end(s));
    }

    Vec vc = {4, 3, 2, 1, 0};
    REQUIRE( v == vc );
}
