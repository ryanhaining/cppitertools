#include "helpers.hpp"
#include <combinations.hpp>

#include <set>
#include <vector>
#include <string>
#include <iterator>

#include "catch.hpp"


using iter::combinations;
using CharCombSet = std::multiset<std::vector<char>>;

TEST_CASE("Simple combination of 4", "[combinations]") {
    std::string s = "ABCD";
    CharCombSet sc;
    for (auto v : combinations(s, 2)) {
        std::vector<char> vcopy(std::begin(v), std::end(v));
        sc.insert(vcopy);
    }

    CharCombSet ans = 
        {{'A','B'}, {'A','C'}, {'A','D'}, {'B','C'}, {'B','D'}, {'C','D'}};
    REQUIRE( ans == sc );
}
        
