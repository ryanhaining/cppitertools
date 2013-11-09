
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
    return 0;
}
