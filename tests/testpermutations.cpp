#include <iostream>
#include <permutations.hpp>
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
    return 0;
}
