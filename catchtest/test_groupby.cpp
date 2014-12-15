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

    const std::vector<std::string> vec = {
        "hi", "ab", "ho",
        "abc", "def",
        "abcde", "efghi"
    };

}

TEST_CASE("groupby: groups words by length") {
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

TEST_CASE("groupby: groups can be skipped completely", "[groupby]") {
    std::vector<int> keys;
    std::vector<std::vector<std::string>> groups;
    for (auto gb : groupby(vec, &length)) {
        if (gb.first == 3) {
            continue;
        }
        keys.push_back(gb.first);
        groups.emplace_back(std::begin(gb.second), std::end(gb.second));
    }

    const std::vector<int> kc = {2, 5};
    REQUIRE( keys == kc );

    const std::vector<std::vector<std::string>> gc = {
        {"hi", "ab", "ho"},
        {"abcde", "efghi"},
    };

    REQUIRE( groups == gc );
}

TEST_CASE("groupby: groups can be skipped partially", "[groupby]") {
    std::vector<int> keys;
    std::vector<std::vector<std::string>> groups;
    for (auto gb : groupby(vec, &length)) {
        keys.push_back(gb.first);
        if (gb.first == 3) {
            std::vector<std::string> cut_short = {*std::begin(gb.second)};
            groups.push_back(cut_short);
        } else {
            groups.emplace_back(std::begin(gb.second), std::end(gb.second));
        }
    }

    const std::vector<int> kc = {2, 3, 5};
    REQUIRE( keys == kc );

    const std::vector<std::vector<std::string>> gc = {
        {"hi", "ab", "ho"},
        {"abc"},
        {"abcde", "efghi"},
    };

    REQUIRE( groups == gc );
}
