#include <groupby.hpp>

#include <vector>
#include <iostream>
#include <string>
#include <functional>

int main() {
    auto len = [](const std::string& str){ return str.size(); };
    std::vector<std::string> vec = {
        "hi", "ab", "ho",
        "abc", "def",
        "abcde", "efghi"
    };

    std::cout << "strings grouped by their length\n";
    for (auto&& gb : iter::groupby(vec, len)) {
        std::cout << "key(" << gb.first << "): ";
        for (auto&& s : gb.second) {
            std::cout << s << "  ";
        }
        std::cout << '\n';
    }

    // groups may be skipped entirely or partially consumed
    std::cout << "skipping length of 3\n";
    for (auto&& gb : iter::groupby(vec, len)) {
        if (gb.first == 3) {
            continue;
        }
        std::cout << "key(" << gb.first << "): ";
        for (auto&& s : gb.second) {
            std::cout << s << "  ";
        }
        std::cout << '\n';
    }


    std::cout << "ints grouped by their value:\n";
    std::vector<int> ivec = {5, 5, 6, 6, 19, 19, 19, 19, 69, 0, 10, 10};
    for (auto&& gb : iter::groupby(ivec)) {
        std::cout << "key(" << gb.first << "): ";
        for (auto&& s : gb.second) {
            std::cout << s << "  ";
        }
        std::cout << '\n';
    }
}
