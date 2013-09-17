#include <filter.hpp>

#include <vector>
#include <iostream>

using iter::filter;

bool greater_than_four(int i) {
    return i > 4;
}

int main() {
    std::vector<int> vec{1, 5, 6, 7, 2, 8, 3, 2, 1};

    std::cout << "Greater than 4\n";
    for (auto i : filter(greater_than_four, vec)) {
        std::cout << i << '\n';
    }

    std::cout << "Less than 4\n";
    for (auto i : filter([] (const int i) { return i < 4; }, vec)) {
        std::cout << i <<'\n';
    }

    return 0;
}
