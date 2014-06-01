#include <accumulate.hpp>
#include <range.hpp>

#include <iostream>
#include <vector>

int main() {
    std::vector<int> vec = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    for (auto v : iter::accumulate(vec, [](int a, int b){return a - b;})) {
        std::cout << v << '\n';
    }
    for (auto v : iter::accumulate(iter::range(10),
                [](int a, int b){return a - b;})) {
        std::cout << v << '\n';
    }
    for (auto v : iter::accumulate({0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
                [](int a, int b){return a - b;})) {
        std::cout << v << '\n';
    }

    for (auto v : iter::accumulate(iter::range(10))) {
        std::cout << v << '\n';
    }
    for (auto v : iter::accumulate({0, 1, 2, 3, 4, 5, 6, 7, 8, 9})) {
        std::cout << v << '\n';
    }

    for (auto v : iter::accumulate(std::vector<int>{1,2,3,4,5,6,7,8,9})) {
        std::cout << v << '\n';
    }

    return 0;
}
