#include <imap.hpp>

#include "helpers.hpp"

#include <vector>
#include <list>
#include <string>
#include <iterator>

#include "catch.hpp"

using iter::starmap;

namespace {
    long f(long d, int i) {
        return d * i;
    }

    std::string g(const std::string& s, int i, char c) {
        std::stringstream ss;
        ss << s << ' ' << i << ' ' << c;
        return ss.str();
    }
}

TEST_CASE("starmap: works with function pointer and lambda", "[starmap]") {
    using Vec = const std::vector<int>;
    const std::vector<std::pair<double, int>> v1 =
        {{1l, 2}, {3l, 11}, {6l, 7}};
    Vec vc = {2l, 33l, 42l};

    SECTION("with function") {
        auto sm = starmap(f, v1);
        Vec v(std::begin(sm), std::end(sm));
        REQUIRE( v == vc );
    }

    SECTION("with lambda") {
        auto sm = starmap([](long a, int b) { return a * b; }, v1);
        Vec v(std::begin(sm), std::end(sm));
        REQUIRE( v == vc );
    }
}

TEST_CASE("starmap: list of tuples", "[starmap]") {
    using Vec = const std::vector<std::string>;
    using T = std::tuple<std::string, int, double>;
    std::list<T> li =
        {T{"hey", 42, 'a'}, T{"there", 3, 'b'}, T{"yall", 5, 'c'}};

    auto sm = starmap(g, li);
    Vec v(std::begin(sm), std::end(sm));
    Vec vc = {"hey 42 a", "there 3 b", "yall 5 c"};

    REQUIRE( v == vc );
}
