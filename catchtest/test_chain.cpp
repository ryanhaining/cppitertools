#include <chain.hpp>

#include <vector>
#include <list>
#include <string>
#include <vector>
#include <iterator>
#include <utility>

#include "catch.hpp"

using iter::chain;
using Vec = const std::vector<char>;

TEST_CASE("chain three strings", "[chain]") {
    std::string s1{"abc"};
    std::string s2{"mno"};
    std::string s3{"xyz"};
    auto ch = chain(s1, s2, s3);

    Vec v(std::begin(ch), std::end(ch));
    Vec vc{'a','b','c','m','n','o','x','y','z'};

    REQUIRE( v == vc );
}

