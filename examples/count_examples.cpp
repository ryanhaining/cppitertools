#include <count.hpp>

#include <iostream>

int main() {
    std::cout << "count() counts towards infinity, breaks at 10\n";
    for (auto i : iter::count()) {
        std::cout << i << '\n';
        if (i == 10) {
            break;
        }
    }

    std::cout << "count(20) starts counting from 20, breaks at 30\n";
    for (auto i : iter::count(20)) {
        std::cout << i << '\n';
        if (i == 30) {
            break;
        }
    }

    std::cout << "count(50, 2) counts by 2 starting at 50, breaks at 70\n";
    for (auto i : iter::count(50, 2)) {
        std::cout << i << '\n';
        if (i >= 70) {
            break;
        }
    }

    std::cout << "count(0, -1) counts down towards -infinity, breaks at -10\n";
    for (auto i : iter::count(0, -1)) {
        std::cout << i << '\n';
        if (i == -10) {
            break;
        }
    }
}
