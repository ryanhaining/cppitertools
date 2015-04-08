#include <count.hpp>

#include <iostream>

using iter::count;

int main() {
    for (auto i : count()) {
        std::cout << i << '\n';
        if (i == 100) {
            break;
        }
    }

    for (auto i : count(5.0, 0.5)){
        std::cout << i << '\n';
        if (i > 100) {
            break;
        }
    }

    for (auto i : count(0, -1)) {
        std::cout << i << '\n';
        if (i < -100) {
            break;
        }
    }

    for (auto i : count()) {
        std::cout << i << '\n';
        if (i > 10000) {
            break;
        }
    }


    return 0;
}
