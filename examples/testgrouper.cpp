#include "grouper.hpp"
#include <iostream>
#include <vector>
using iter::grouper;
int main() {
    std::vector<int> v {1,2,3,4,5,6,7,8,9}; 
    for (auto sec : grouper(v,4)) {
        for (auto i : sec) {
            std::cout << i << " ";
            i.get() *= 2;
        }
        std::cout << '\n';
    }

    for (auto sec : grouper(std::vector<int>{1,2,3,4,5,6,7,8,9}, 4)) {
        for (auto i : sec) {
            std::cout << i << " ";
            i.get() *= 2;
        }
        std::cout << '\n';
    }

    for (auto sec : grouper(v,3)) {
        for (auto i : sec) {
            std::cout << i << " ";
        }
        std::cout << '\n';
    }
    std::vector<int> empty {};
    for (auto sec : grouper(empty,3)) {
        std::cout << "Shouldn't print\n";
        for (auto i : sec) {
            std::cout << i << " Shouldn't print\n";
        }
    }

    int arr[] = {1,2,3,4,5,6,7};
    for (auto sec : grouper(arr, 2)) {
        for (auto i : sec) {
            std::cout << i << ' ';
        }
        std::cout << '\n';
    }

    for (auto sec : grouper({1,2,3,4,5,6,7}, 2)) {
        for (auto i : sec) {
            std::cout << i << ' ';
        }
        std::cout << '\n';
    }
}
