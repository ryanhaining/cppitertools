#include <filter.hpp>

#include "helpers.hpp"

#include <vector>
#include <string>
#include <iterator>

#include "catch.hpp"

using iter::filter;

using Vec = const std::vector<int>;

namespace {
    bool less_than_five(int i) {
        return i < 5;
    }

    class LessThanValue {
        private:
            int compare_val;

        public:
            LessThanValue(int v) : compare_val(v) { }

            bool operator() (int i) {
                return i < this->compare_val;
            }
    };
}

TEST_CASE("filter: handles different functor types", "[filter]") {
    Vec ns = {1,2, 5,6, 3,1, 7, -1, 5};
    Vec vc = {1,2,3,1,-1};
    SECTION("with function pointer") {
        auto f = filter(less_than_five, ns);
        Vec v(std::begin(f), std::end(f));
        REQUIRE( v == vc );
    }

    SECTION("with callable object") {
        auto f = filter(LessThanValue{5}, ns);
        Vec v(std::begin(f), std::end(f));
        REQUIRE( v == vc );
    }

    SECTION("with lambda") {
        auto ltf = [](int i) {return i < 5;};
        auto f = filter(ltf, ns);
        Vec v(std::begin(f), std::end(f));
        REQUIRE( v == vc );
    }
}

TEST_CASE("filter: using identity", "[filter]") {
    Vec ns{0, 1, 2, 0, 3, 0, 0, 0, 4, 5, 0};
    Vec vc = {1,2,3,4,5};
    auto f = filter(ns);
    Vec v(std::begin(f), std::end(f));
    REQUIRE( v == vc );
}
