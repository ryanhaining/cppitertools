
#include <vector>
#include <iostream>

#include <unique_justseen.hpp>
using iter::unique_justseen;

int main() {
    std::vector<int> v {1,1,1,2,2,4,4,5,6,7,8,8,8,8,9,9};
    for (auto i : unique_justseen(v)) {
        std::cout << i << " ";
    }std::cout << std::endl;
    return 0;
}
