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
    for (auto&& gb : groupby(vec, &length)) {
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
    for (auto&& gb : groupby(vec, &length)) {
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
    for (auto&& gb : groupby(vec, &length)) {
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

TEST_CASE("groupby: single argument uses elements as keys", "[groupby]") {
    std::vector<int> ivec = {5, 5, 6, 6, 19, 19, 19, 19, 69, 0, 10, 10};
    std::vector<int> keys;
    std::vector<std::vector<int>> groups;
    for (auto&& gb : groupby(ivec)) {
        keys.push_back(gb.first);
        groups.emplace_back(std::begin(gb.second), std::end(gb.second));
    }

    const std::vector<int> kc = {5, 6, 19, 69, 0, 10};
    REQUIRE( keys == kc );

    std::vector<std::vector<int>> gc = {
        {5, 5},
        {6, 6},
        {19, 19, 19, 19},
        {69},
        {0},
        {10, 10},
    };

    REQUIRE( groups == gc );
}

TEST_CASE("groupby: empty iterable yields nothing", "[groupby]") {
    std::vector<int> ivec{};
    auto g = groupby(ivec);
    REQUIRE( std::begin(g) == std::end(g) );
}

TEST_CASE("groupby: inner iterator (group) not used", "[groupby]") {
    std::vector<int> keys;
    for (auto&& gb : groupby(vec, length)) {
        keys.push_back(gb.first);
    }
    
    std::vector<int> kc = {2, 3, 5};
    REQUIRE( keys == kc );
}
