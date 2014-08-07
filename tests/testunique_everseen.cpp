
#include <vector>
#include <iostream>

#include <unique_everseen.hpp>
using iter::unique_everseen;

int main() {
    {
        //should work same as justseen here
        std::vector<int> v {1,1,1,2,2,3,4,4,5,6,7,8,8,8,8,9,9};
        for (auto i : unique_everseen(v)) {
            std::cout << i << " ";
        }std::cout << std::endl;
    }
    {
        std::vector<int> v {1,2,3,4,3,2,1,5,6,7,7,8,9,8,9,6};
        for (auto i : unique_everseen(v)) {
            std::cout << i << " ";
        }std::cout << std::endl;
    }

    for (auto i : unique_everseen(
                std::vector<int>{1,2,1,3,4,3,2,1,5,6,7,7,8,9,8,9,6})) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    int arr[] = {1,2,1,3,4,3,2,1,5,6,7,7,8,9,8,9,6};
    for (auto i : unique_everseen(arr)) {
        std::cout << i << ' ';
    }
    std::cout << '\n';

    for (auto i : unique_everseen({1,2,1,3,4,3,2,1,5,6,7,7,8,9,8,9,6})) {
        std::cout << i << ' ';
    }
    std::cout << '\n';

    return 0;
}
