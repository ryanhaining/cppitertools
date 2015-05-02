#include <takewhile.hpp>

#include <vector>
#include <iostream>

int main() {
    std::vector<int> ivec{1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1};
    std::cout << "take ints as long as they are less than 5: ";
    for (auto&& i : iter::takewhile([] (int i) {return i < 5;}, ivec)) {
        std::cout << i << ' ';
    }
    std::cout << '\n';
}
