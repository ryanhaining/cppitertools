#include <zip_longest.hpp>

#include "helpers.hpp"

#include <vector>
#include <tuple>
#include <string>
#include <iterator>
#include <utility>
#include <iterator>
#include <sstream>
#include <iostream>

#include "catch.hpp"

using iter::zip_longest;

// reopening boost is the only way I can find that gets this to print
namespace boost {
template <typename T>
std::ostream& operator<<(std::ostream& out, const optional<T>& opt) {
    if (opt) {
        out << "Just " << *opt;
    } else {
        out << "Nothing";
    }
    return out;
}
}

template <typename... Ts>
using const_opt_tuple = std::tuple<boost::optional<const Ts&>...>;

TEST_CASE("zip longest: correctly detects longest at any position",
        "[zip_longest]") {

    const std::vector<int> ivec{2, 4, 6, 8, 10, 12};
    const std::vector<std::string> svec{"abc", "def", "xyz"};
    const std::string str{"hello"};

    SECTION("longest first") {
        using TP = const_opt_tuple<int, std::string, char>;
        using ResVec = std::vector<TP>;

        auto zl = zip_longest(ivec, svec, str);
        ResVec results(std::begin(zl), std::end(zl));
        ResVec rc = {
            TP{{ivec[0]},  {svec[0]},   {str[0]}},
            TP{{ivec[1]},  {svec[1]},   {str[1]}},
            TP{{ivec[2]},  {svec[2]},   {str[2]}},
            TP{{ivec[3]},  {},          {str[3]}},
            TP{{ivec[4]},  {},          {str[4]}},
            TP{{ivec[5]},  {},          {}    }
        };

        REQUIRE( results == rc );
    }

    SECTION("longest in middle") {
        using TP = const_opt_tuple<std::string, int, char>;
        using ResVec = std::vector<TP>;

        auto zl = zip_longest(svec, ivec, str);
        ResVec results(std::begin(zl), std::end(zl));
        ResVec rc = {
            TP{{svec[0]},   {ivec[0]},  {str[0]}},
            TP{{svec[1]},   {ivec[1]},  {str[1]}},
            TP{{svec[2]},   {ivec[2]},  {str[2]}},
            TP{{},          {ivec[3]},  {str[3]}},
            TP{{},          {ivec[4]},  {str[4]}},
            TP{{},          {ivec[5]},  {}    }
        };

        REQUIRE( results == rc );
    }

    SECTION("longest at end") {
        using TP = const_opt_tuple<std::string, char, int>;
        using ResVec = std::vector<TP>;

        auto zl = zip_longest(svec, str, ivec);
        ResVec results(std::begin(zl), std::end(zl));
        ResVec rc = {
            TP{{svec[0]},   {str[0]},   {ivec[0]}},
            TP{{svec[1]},   {str[1]},   {ivec[1]}},
            TP{{svec[2]},   {str[2]},   {ivec[2]}},
            TP{{},          {str[3]},   {ivec[3]}},
            TP{{},          {str[4]},   {ivec[4]}},
            TP{{},          {},         {ivec[5]}}
        };

        REQUIRE( results == rc );
    }
}
