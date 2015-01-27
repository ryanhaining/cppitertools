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
template <typename T>
using opt = boost::optional<T>;

TEST_CASE("zip longest: correctly detects longest at any position",
        "[zip_longest]") {
    using TP = std::tuple<opt<const int&>, opt<const std::string&>, opt<const char&>>;
    using ResVec = std::vector<TP>;

    const std::vector<int> ivec{2, 4, 6, 8, 10, 12};
    const std::vector<std::string> svec{"abc", "def", "xyz"};
    const std::string str{"hello"};

    ResVec results;
    ResVec rc;

    SECTION("longest first") {
        auto zl = zip_longest(ivec, svec, str);
        results = ResVec(std::begin(zl), std::end(zl));
        rc = ResVec {
            TP{ivec[0], svec[0], str[0]},
            TP{ivec[1], svec[1], str[1]},
            TP{ivec[2], svec[2], str[2]},
            TP{ivec[3], {},      str[3]},
            TP{ivec[4], {},      str[4]},
            TP{ivec[5], {},      {}    }
        };
    }

    REQUIRE( results == rc );
}
