#include <batched.hpp>

#include <iostream>
#include <vector>

int main() {
    std::vector<int> v {1,2,3,4,5,6,7,8,9}; 
    std::cout << "num batches: 5\n";
    for (auto&& sec : iter::batched(v, 5)) {
        for (auto&& i : sec) {
            std::cout << i << " ";
        }
        std::cout << '\n';
    }

    std::cout << "num batches: 4\n";
    for (auto&& sec : iter::batched(v, 4)) {
        for (auto&& i : sec) {
            std::cout << i << " ";
        }
        std::cout << '\n';
    }

    std::cout << "num batches: 6\n";
    for (auto&& sec : iter::batched(v, 6)) {
        for (auto&& i : sec) {
            std::cout << i << " ";
        }
        std::cout << '\n';
    }
}
