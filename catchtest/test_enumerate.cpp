#include <enumerate.hpp>

#include <vector>
#include <string>
#include <iterator>
#include <utility>

#include "catch.hpp"

using Vec = std::vector<std::pair<std::size_t, char>>;
using iter::enumerate;

TEST_CASE("Basic Function", "[enumerate]") {
    std::string str = "abc";
    auto e = enumerate(str);
    Vec v(std::begin(e), std::end(e));
    Vec vc{{0, 'a'}, {1, 'b'}, {2, 'c'}};

    REQUIRE( v == vc );
}

TEST_CASE("Empty", "[enumerate]") {
    std::string emp{};
    auto e = enumerate(emp);
    Vec v(std::begin(e), std::end(e));
    REQUIRE( v.empty() );
}
