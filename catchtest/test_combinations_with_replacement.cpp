#include <combinations_with_replacement.hpp>

#include <iterator>
#include <string>
#include <set>
#include <vector>

#include "helpers.hpp"
#include "catch.hpp"

using iter::combinations_with_replacement;
using itertest::BasicIterable;
using CharCombSet = std::multiset<std::vector<char>>;

TEST_CASE("combinations_with_replacement: Simple combination",
        "[combinations_with_replacement]") {
    std::string s{"ABC"};
    CharCombSet sc;
    for (auto v : combinations_with_replacement(s, 2)) {
        std::vector<char> vcopy(std::begin(v), std::end(v));
        sc.insert(vcopy);
    }
    CharCombSet ans =
        {{'A','A'}, {'A','B'}, {'A','C'}, {'B','B'}, {'B','C'}, {'C','C'}};
    REQUIRE( ans == sc );
}



TEST_CASE("combinations_with_replacement: big size is no problem",
        "[combinations_with_replacement]") {
    std::string s{"AB"};
    CharCombSet sc;
    for (auto v : combinations_with_replacement(s, 3)) {
        std::vector<char> vcopy(std::begin(v), std::end(v));
        sc.insert(vcopy);
    }
    CharCombSet ans =
        {{'A', 'A', 'A'}, {'A', 'A', 'B'}, {'A', 'B', 'B'}, {'B', 'B', 'B'}};
    REQUIRE( ans == sc );
}
    
