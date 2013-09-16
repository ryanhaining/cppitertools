#include "enumerate.hpp"

#include <string>
#include <iostream>
#include <vector>

using iter::enumerate;

int main() {
    std::string s = "hello world";

    const std::string const_string("goodbye world");
    for (auto e : enumerate(const_string)) {
        std::cout << e.index << ": " << e.element << std::endl;
    }

    std::vector<int> vec;
    for(int i = 0; i < 12; ++i) { 
        vec.push_back(i * i);
    }

    for (auto e : enumerate(vec)) { 
        std::cout << e.index << ": " << e.element << std::endl;
        e.element = 0;
        // tests to make sure vector can be edited
        std::cout << e.index << ": " << e.element << std::endl;
    }

    return 0;
}
