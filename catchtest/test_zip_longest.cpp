#include <zip_longest.hpp>

#include "helpers.hpp"

#include <vector>
#include <tuple>
#include <string>
#include <iterator>
#include <utility>
#include <iterator>

#include "catch.hpp"

using iter::zip_longest;

TEST_CASE("zip longest: correctly detects longest at any position",
        "[zip_longest]") {
    using TP = std::tuple<int, std::string, char>;
    using ResVec = std::vector<TP>;

    const std::vector<int> ivec{2, 4, 6, 8, 10, 12};
    const std::vector<std::string> svec{"abc", "def", "xyz"};
    const std::string str{"hello"};

    ResVec results;
    ResVec rc;

    SECTION("longest first") {
        for (auto&& t : zip_longest(ivec, svec, str)) {
            results.emplace_back(
                    std::get<0>(t).get_value_or(-1),
                    std::get<1>(t).get_value_or(""),
                    std::get<2>(t).get_value_or('\0')
            );
        }
        rc = ResVec {
            TP{2, "abc", 'h'},
                TP{4, "def", 'e'},
                TP{6, "xyz", 'l'},
                TP{8, "", 'l'},
                TP{10, "", 'o'},
                TP{12, "", '\0'}
        };
    }


    REQUIRE( results == rc );
}
