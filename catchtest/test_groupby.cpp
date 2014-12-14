#include <groupby.hpp>

#include "helpers.hpp"

#include <vector>
#include <string>
#include <iterator>

#include "catch.hpp"

using iter::groupby;

namespace {
    int length(const std::string& s) {
        return s.size();
    }
}

TEST_CASE("groupby: groups words by length") {
    const std::vector<std::string> vec = {
        "hi", "ab", "ho",
        "abc", "def",
        "abcde", "efghi"
    };

    std::vector<int> keys;
    std::vector<std::vector<std::string>> groups;
    for (auto gb : groupby(vec, &length)) {
        keys.push_back(gb.first);
        groups.emplace_back(std::begin(gb.second), std::end(gb.second));
    }

    const std::vector<int> kc = {2, 3, 5};
    REQUIRE( keys == kc );

    const std::vector<std::vector<std::string>> gc = {
        {"hi", "ab", "ho"},
        {"abc", "def"},
        {"abcde", "efghi"},
    };

    REQUIRE( groups == gc );
}
