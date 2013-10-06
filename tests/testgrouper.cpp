#include "grouper.hpp"
#include <iostream>
#include <vector>
using iter::grouper;
int main() {
    std::vector<int> v = {1,2,3,4,5,6,7,8,9}; 
    for (auto sec : grouper(v,4)) {
        for (auto i : sec) {
            std::cout << i << " ";
            //i.get() = 90;
        }
        std::cout << std::endl;
    }
    return 0;
}
