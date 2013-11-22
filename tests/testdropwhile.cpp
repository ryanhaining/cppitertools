#include <dropwhile.hpp>
#include <range.hpp>

#include <vector>
#include <iostream>

using iter::dropwhile;
using iter::range;

int main() {
    std::vector<int> ivec{1, 2, 3, 4, 5, 6, 7, 8, 1, 2, 3, 4};
    for (auto i : dropwhile([] (int i) {return i < 5;}, ivec)) {
        std::cout << i << '\n';
    }

    for (auto i : dropwhile([] (int i) {return i < 5;}, range(10))) {
        std::cout << i << '\n';
    }

    for (auto i : dropwhile([] (int i) {return i < 5;}, 
                {1, 2, 3, 4, 5, 6, 7, 8, 9})) {
        std::cout << i << '\n';
    }

    return 0;
}
