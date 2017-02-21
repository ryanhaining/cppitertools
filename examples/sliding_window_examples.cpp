#include "sliding_window.hpp"

#include <iostream>
#include <vector>

int main() {
    std::vector<int> v = {1,2,3,4,5,6,7,8,9}; 
    for (auto&& sec : iter::sliding_window(v,4)) {
        for (auto&& i : sec) {
            std::cout << i << ' ';
        }
        std::cout << '\n';
    }

    std::cout << "Empty when window size is > length\n";
    for (auto&& sec : iter::sliding_window(std::vector<int>{1,2,3}, 10)) {
        for (auto&& i : sec) {
            std::cout << i << ' ';
        }
        std::cout << '\n';
    }
}
