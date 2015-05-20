#include <accumulate.hpp>
#include <range.hpp>

#include <iostream>
#include <vector>

int main() {
    std::vector<int> vec = {0, 1, 2, 3, 4, 5};
    // accumulate adds elements by default
    std::cout << "accumulate({1, 2, 3, 4, 5}): { ";
    for (auto i : iter::accumulate(vec)) {
        std::cout << i << ' ';
    }
    std::cout << "}\n";

    // accumulate with a lambda for subtraction
    std::cout << "accumulate({1, 2, 3, 4, 5}, subtract): { ";
    for (auto i : iter::accumulate(vec, [](int a, int b){return a - b;})) {
        std::cout << i << ' ';
    }
    std::cout << "}\n";
}
