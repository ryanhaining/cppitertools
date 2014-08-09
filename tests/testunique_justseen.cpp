
#include <vector>
#include <iostream>

#include <unique_justseen.hpp>
using iter::unique_justseen;

int main() {
    std::vector<int> v {1,1,1,2,2,4,4,5,6,7,8,8,8,8,9,9};
    for (auto i : unique_justseen(v)) {
        std::cout << i << " ";
    }
    std::cout << '\n';

    std::cout << "with temporary\n";
    for (auto i : unique_justseen(std::vector<int>{1,1,1,2,3,3})) {
        std::cout << i << " ";
    }
    std::cout << '\n';

    std::cout << "with init list\n";
    for (auto i : unique_justseen({1,1,1,2,3,3})) {
        std::cout << i << " ";
    }
    std::cout << '\n';

    std::cout << "with static array\n";
    int arr[] = {1, 1, 2, 3, 3, 3, 4};
    for (auto i : unique_justseen(arr)) {
        std::cout << i << " ";
    }
    std::cout << '\n';

    return 0;
}
