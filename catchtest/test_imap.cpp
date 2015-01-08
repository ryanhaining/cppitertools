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
}

TEST_CASE("imap: works with lambda, callable, and function", "[imap]") {
    std::vector<int> ns = {10, 20, 30};
    SECTION("with lambda") {
        auto im = imap([](int i) { return i + 1; }, ns);
        Vec v(std::begin(im), std::end(im));
        Vec vc = {11, 21, 31};
        REQUIRE(v == vc);
    }

    SECTION("with function") {
        auto im = imap(plusone, ns);
        Vec v(std::begin(im), std::end(im));
        Vec vc = {11, 21, 31};
        REQUIRE(v == vc);
    }

    SECTION("with function") {
        auto im = imap(PlusOner{}, ns);
        Vec v(std::begin(im), std::end(im));
        Vec vc = {11, 21, 31};
        REQUIRE(v == vc);
    }

}
