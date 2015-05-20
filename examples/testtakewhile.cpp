#include <takewhile.hpp>
#include <range.hpp>

#include <vector>
#include <iostream>

using iter::takewhile;
using iter::range;

int main() {
    std::vector<int> ivec{1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1};
    for (auto i : takewhile([] (int i) {return i < 5;}, ivec)) {
        std::cout << i << '\n';
    }

    for (auto i : takewhile([] (int i) {return i < 5;}, range(10))) {
        std::cout << i << '\n';
    }

    for (auto i : takewhile([] (int i) {return i < 5;}, 
                {1, 2, 3, 4, 5, 6, 7, 8, 9})) {
        std::cout << i << '\n';
    }

    std::cout << "with temporary\n";
    for (auto i : takewhile([] (int i) {return i < 5;}, 
                std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9})) {
        std::cout << i << '\n';
    }

    return 0;
}
