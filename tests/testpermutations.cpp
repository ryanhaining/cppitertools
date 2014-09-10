#include "samples.hpp"

#include <permutations.hpp>
#include <range.hpp>

#include <iostream>
#include <vector>
#include <string>

int main() {
    using iter::permutations;
    std::vector<int> v = {1,2,3};
    for (auto vec : permutations(v)) {
        for (auto i : vec) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
    }
    //try with string
    std::string s = "aba";
    for (auto vec : permutations(s)) {
        for (auto c : vec) {
            std::cout << c << " ";
        }
        std::cout << std::endl;
    }
    s = "abc";
    for (auto vec : permutations(s)) {
        for (auto c : vec) {
            std::cout << c << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "init list\n";
    //std::next_permutation doesn't work on initializer_lists
    for (auto vec : permutations({1,2,3,4})) {
        for (auto c : vec) {
            std::cout << c << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "with container of move-only objects\n";
    std::vector<itertest::MoveOnly> mv;
    for (auto i : iter::range(3)) {
        mv.emplace_back(i);
    }
    for (auto v : permutations(mv)) {
        for (auto i : v) std::cout << i << " ";
        std::cout << std::endl;
    }

    std::cout << "with deref-by-value iterator\n";
    itertest::DerefByValue dbv;
    for (auto v : permutations(dbv)) {
        for (auto i : v) std::cout << i << " ";
        std::cout << std::endl;
    }
}
