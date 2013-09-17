#include <cycle.hpp>

#include <vector>
#include <iostream>

using iter::cycle;

int main() {
    std::vector<int> vec;
    vec.push_back(2);
    vec.push_back(4);
    vec.push_back(6);

    for (auto i : cycle(vec)) {
        std::cout << i << '\n';
    }

    return 0;
}
