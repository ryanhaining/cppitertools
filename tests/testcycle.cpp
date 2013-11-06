#include <cycle.hpp>

#include <vector>
#include <iostream>

using iter::cycle;

int main() {
    std::vector<int> vec = {2, 4, 6};

    size_t count = 0;
    for (auto i : cycle(vec)) {
        std::cout << i << '\n';
        if (count == 100) {
            break;
        }
        ++count;
    }

    return 0;
}
