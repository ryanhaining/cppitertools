#include <powerset.hpp>
#include <vector>
#include <iostream>

using iter::powerset;

int main() {
    std::vector<int> vec {1,2,3,4,5,6,7,8,9};
    for (auto v : powerset(vec)) {
        for (auto i : v) std::cout << i << " ";
        std::cout << std::endl;
    }
    for (auto v : powerset({1,2,3,4})) {
        for (auto i : v) std::cout << i << " ";
        std::cout << std::endl;
    }

    return 0;
}
