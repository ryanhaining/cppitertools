#include <enumerate.hpp>

#include "helpers.hpp"

#include <vector>
#include <string>
#include <iterator>
#include <utility>
#include <sstream>

namespace Catch {
template <typename A, typename B>
std::string toString( const std::pair<A, B>& p) {
    std::ostringstream oss;
    oss << '{' << p.first << ", " << p.second << '}';
    return oss.str();
}
}

#include "catch.hpp"

using Vec = std::vector<std::pair<std::size_t, char>>;
using iter::enumerate;

using itertest::BasicIterable;
using itertest::SolidInt;

TEST_CASE("Basic Functioning enumerate", "[enumerate]") {
    std::string str = "abc";
    auto e = enumerate(str);
    Vec v(std::begin(e), std::end(e));
    Vec vc{{0, 'a'}, {1, 'b'}, {2, 'c'}};

    REQUIRE( v == vc );
}

TEST_CASE("Empty enumerate", "[enumerate]") {
    std::string emp{};
    auto e = enumerate(emp);
    REQUIRE( std::begin(e) == std::end(e) );
}

TEST_CASE("Postfix ++ enumerate", "[enumerate]") {
    std::string s{"amz"};
    auto e = enumerate(s);
    auto it = std::begin(e);
    it++;
    REQUIRE( (*it).first == 1 );
}


TEST_CASE("Modifications through enumerate affect container", "[enumerate]") {
    std::vector<int> v{1, 2, 3, 4};
    std::vector<int> vc(v.size(), -1);
    for (auto&& p : enumerate(v)){
        p.second = -1;
    }

    REQUIRE( v == vc );
}

TEST_CASE("enumerate with static array works", "[enumerate]") {
    char arr[] = {'w', 'x', 'y'};

    SECTION("Conversion to vector") {
        auto e = enumerate(arr);
        Vec v(std::begin(e), std::end(e));
        Vec vc{{0, 'w'}, {1, 'x'}, {2, 'y'}};
        REQUIRE( v == vc );
    }

    SECTION("Modification through enumerate") {
        for (auto&& p : enumerate(arr)) {
            p.second = 'z';
        }
        std::vector<char> v(std::begin(arr), std::end(arr));
        decltype(v) vc(v.size(), 'z');
        REQUIRE( v == vc );
    }
}

TEST_CASE("initializer_list works", "[enumerate]") {
    auto e = enumerate({'a', 'b', 'c'});
    Vec v(std::begin(e), std::end(e));
    Vec vc{{0, 'a'}, {1, 'b'}, {2, 'c'}};
    REQUIRE( v == vc );
}

TEST_CASE("binds reference when it should", "[enumerate]") {
    BasicIterable<char> bi{'x', 'y', 'z'};
    auto e = enumerate(bi);
    (void)e;
    REQUIRE_FALSE( bi.was_moved_from() );
}

TEST_CASE("moves rvalues into enumerable object", "[enumerate]") {
    BasicIterable<char> bi{'x', 'y', 'z'};
    auto e = enumerate(std::move(bi));
    REQUIRE( bi.was_moved_from());
    (void)e;
}

TEST_CASE("Works with const iterable", "[enumerate]") {
    const std::string s{"ace"};
    auto e = enumerate(s);
    Vec v(std::begin(e), std::end(e));
    Vec vc{{0, 'a'}, {1, 'c'}, {2, 'e'}};
    REQUIRE( v == vc );
}

TEST_CASE("Doesn't move or copy elements of iterable", "[enumerate]") {
    constexpr SolidInt arr[] = {{6}, {7}, {8}};
    for (auto&& i : enumerate(arr)) {
        (void)i;
    }
}


TEST_CASE("enumerate: iterator meets requirements", "[enumerate]") {
    std::string s{};
    auto c = enumerate(s);
    REQUIRE( itertest::IsIterator<decltype(std::begin(c))>::value );
}
