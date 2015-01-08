#include <imap.hpp>

#include "helpers.hpp"

#include <vector>
#include <string>
#include <iterator>

#include "catch.hpp"

using iter::imap;
using Vec = const std::vector<int>;

namespace {
    int plusone(int i) {
        return i + 1;
    }

    class PlusOner {
        public:
            int operator()(int i) {
                return i + 1;
            }
    };

    int power(int b, int e) {
        int acc = 1;
        for (int i = 0; i < e; ++i) {
            acc *= b;
        }
        return acc;
    }
}

TEST_CASE("imap: works with lambda, callable, and function", "[imap]") {
    std::vector<int> ns = {10, 20, 30};
    SECTION("with lambda") {
        auto im = imap([](int i) { return i + 1; }, ns);
        Vec v(std::begin(im), std::end(im));
        Vec vc = {11, 21, 31};
        REQUIRE( v == vc );
    }

    SECTION("with function") {
        auto im = imap(plusone, ns);
        Vec v(std::begin(im), std::end(im));
        Vec vc = {11, 21, 31};
        REQUIRE( v == vc );
    }

    SECTION("with function") {
        auto im = imap(PlusOner{}, ns);
        Vec v(std::begin(im), std::end(im));
        Vec vc = {11, 21, 31};
        REQUIRE( v == vc );
    }

}

TEST_CASE("imap: works with multiple sequences", "[imap]") {
    Vec bases = {0, 1, 2, 3};
    Vec exps = {1, 2, 3, 4};

    auto im = imap(power, bases, exps);
    Vec v(std::begin(im), std::end(im));
    Vec vc = {0, 1, 8, 81};

    REQUIRE( v == vc );
}

TEST_CASE("imap: terminates on shortest squence", "[imap]") {
    Vec ns1 = {1, 2, 3, 4};
    Vec ns2 = {2, 4, 6, 8, 10};
    Vec vc = {3, 6, 9, 12};
    SECTION("shortest sequence first") {
        auto im = imap([](int a, int b){ return a + b; }, ns1, ns2);
        Vec v(std::begin(im), std::end(im));
        REQUIRE( v == vc );
    }
    SECTION("shortest sequence second") {
        auto im = imap([](int a, int b){ return a + b; }, ns2, ns1);
        Vec v(std::begin(im), std::end(im));
        REQUIRE( v == vc );
    }
}
