#include <dropwhile.hpp>

#include <vector>
#include <iostream>

using iter::dropwhile;

int main() {
    std::vector<int> ivec{1, 2, 3, 4, 5, 6, 7, 8, 1, 2, 3, 4};
    for (auto i : dropwhile([] (int i) {return i < 5;}, ivec)) {
        std::cout << i << '\n';
    }
    return 0;
}
