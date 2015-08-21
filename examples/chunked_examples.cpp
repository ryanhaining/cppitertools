#include <chunked.hpp>

#include <iostream>
#include <vector>

int main() {
    std::cout << "chunk size: 4\n";
    std::vector<int> v {1,2,3,4,5,6,7,8,9}; 
    for (auto&& sec : iter::chunked(v, 4)) {
        for (auto&& i : sec) {
            std::cout << i << " ";
        }
        std::cout << '\n';
    }

    std::cout << "chunk size: 3\n";
    for (auto&& sec : iter::chunked(v,3)) {
        for (auto&& i : sec) {
            std::cout << i << " ";
        }
        std::cout << '\n';
    }
}
