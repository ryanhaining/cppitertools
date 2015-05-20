#include <unique_justseen.hpp>

#include <vector>
#include <iostream>

int main() {
    std::cout << "omits consecutive duplicates: ";
    std::vector<int> v = {1,1,1,2,2,4,4,5,6,7,8,8,8,8,9,9};
    for (auto&& i : iter::unique_justseen(v)) {
        std::cout << i << ' ';
    }
    std::cout << '\n';

    std::cout << "doesn't omit non-consecutive duplicates: ";
    std::vector<int> v2 = {1,2,3,2,1,2,3};
    for (auto&& i : iter::unique_justseen(v2)) {
        std::cout << i << ' ';
    }
    std::cout << '\n';
}
