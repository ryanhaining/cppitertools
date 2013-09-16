#include "chain.hpp"

#include <vector>
#include <string>
#include <iostream>

using iter::chain;

int main() {
    std::vector<int> ivec{1, 4, 7, 9};
    std::vector<int> lvec{100, 200, 300, 400, 500, 600};

    for (auto e : chain(ivec, lvec)) {
        std::cout << e << std::endl;
    }

    return 0;
}
