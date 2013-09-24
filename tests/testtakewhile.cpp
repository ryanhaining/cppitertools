#include <takewhile.hpp>

#include <vector>
#include <iostream>

using iter::takewhile;

int main() {
    std::vector<int> ivec{1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1};
    for (auto i : takewhile([] (int i) {return i < 5;}, ivec)) {
        std::cout << i << '\n';
    }
    return 0;
}
