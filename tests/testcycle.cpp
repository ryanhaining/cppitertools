#include <cycle.hpp>
#include <range.hpp>

#include <vector>
#include <iostream>

using iter::cycle;
using iter::range;

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

    count = 0;
    int array[] = {68, 69, 70};
    for (auto i : cycle(array)) {
        std::cout << i << '\n';
        if (count == 20) {
            break;
        }
        ++count;
    }

    count = 0;
    for (auto i : cycle({7, 8, 9})) {
        std::cout << i << '\n';
        if (count == 20) {
            break;
        }
        ++count;
    }

    count = 0;
    for (auto i : cycle(range(3))) {
        std::cout << i << '\n';
        if (count == 20) {
            break;
        }
        ++count;
    }

    count = 0;
    const std::string s("hello");
    for (auto c : cycle(s)) {
        std::cout << c << '\n';
        if (count == 20) {
            break;
        }
        ++count;
    }

    count = 0;
    for (auto i : std::vector<int>{1,2,3,4,5}) {
        std::cout << i << '\n';
        if (count == 20) {
            break;
        }
        ++count;
    }

    return 0;
}
