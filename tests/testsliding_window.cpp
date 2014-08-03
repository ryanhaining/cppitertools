#include "sliding_window.hpp"

#include <iostream>
#include <vector>

using iter::sliding_window;

int main() {
    std::vector<int> v = {1,2,3,4,5,6,7,8,9}; 
    for (auto sec : sliding_window(v,4)) {
        for (auto i : sec) {
            std::cout << i << " ";
            i.get() = 90;
        }
        std::cout << std::endl;
    }

    std::cout << "with temporary\n";
    for (auto sec : sliding_window(std::vector<int>{1,2,3,4,5,6,7,8,9}, 4)) {
        for (auto i : sec) {
            std::cout << i << " ";
            i.get() = 90;
        }
        std::cout << std::endl;
    }

    std::cout << "with init list\n";
    for (auto sec : sliding_window({1,2,3,4,5,6,7,8,9}, 4)) {
        for (auto i : sec) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "with window_size > length\n";
    for (auto sec : sliding_window({1,2,3}, 10)) {
        for (auto i : sec) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "with static array\n";
    int arr[] = {1,2,3,4,5,6,7,8,9};
    for (auto sec : sliding_window(arr, 4)) {
        for (auto i : sec) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
