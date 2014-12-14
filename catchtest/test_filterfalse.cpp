#include <filterfalse.hpp>

#include "helpers.hpp"

#include <vector>
#include <string>
#include <iterator>

#include "catch.hpp"

using iter::filterfalse;

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

TEST_CASE("filterfalse: handles different functor types", "[filterfalse]") {
    Vec ns = {1,2, 5,6, 3,1, 7, -1, 5};
    Vec vc = {5,6,7,5};
    SECTION("with function pointer") {
        auto f = filterfalse(less_than_five, ns);
        Vec v(std::begin(f), std::end(f));
        REQUIRE( v == vc );
    }

    SECTION("with callable object") {
        auto f = filterfalse(LessThanValue{5}, ns);
        Vec v(std::begin(f), std::end(f));
        REQUIRE( v == vc );
    }

    SECTION("with lambda") {
        auto ltf = [](int i) {return i < 5;};
        auto f = filterfalse(ltf, ns);
        Vec v(std::begin(f), std::end(f));
        REQUIRE( v == vc );
    }
}

TEST_CASE("filterfalse: using identity", "[filterfalse]") {
    Vec ns{0, 1, 2, 0, 3, 0, 0, 0, 4, 5, 0};
    auto f = filterfalse(ns);
    Vec v(std::begin(f), std::end(f));

    Vec vc = {0, 0, 0, 0, 0, 0};
    REQUIRE( v == vc );
}

TEST_CASE("filterfalse: binds to lvalues, moves rvales", "[filterfalse]") {
    itertest::BasicIterable<int> bi{1,2,3,4};

    SECTION("one-arg binds to lvalues") {
        filterfalse(bi);
        REQUIRE_FALSE(bi.was_moved_from());
    }

    SECTION("two-arg binds to lvalues") {
        filterfalse(less_than_five, bi);
        REQUIRE_FALSE(bi.was_moved_from());
    }

    SECTION("one-arg moves rvalues") {
        filterfalse(std::move(bi));
        REQUIRE(bi.was_moved_from());
    }

    SECTION("two-arg moves rvalues") {
        filterfalse(less_than_five, std::move(bi));
        REQUIRE(bi.was_moved_from());
    }
}
