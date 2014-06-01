#include <enumerate.hpp>
#include <range.hpp>

#include <string>
#include <iostream>
#include <vector>

using iter::enumerate;
using iter::range;

int main() {
    std::cout << "const std::string\n";
    const std::string const_string("goodbye world");
    for (auto e : enumerate(const_string)) {
        std::cout << e.index << ": " << e.element << std::endl;
    }


    std::vector<int> vec;
    for(int i = 0; i < 12; ++i) { 
        vec.push_back(i * i);
    }


    std::cout << "print vector element, set it to zero, then print it again\n";
    for (auto e : enumerate(vec)) { 
        std::cout << e.index << ": " << e.element << std::endl;
        e.element = 0;
        // tests to make sure vector can be edited
        std::cout << e.index << ": " << e.element << std::endl;
    }

    std::cout << "static array\n";
    int array[] = {1, 9, 8, 11};
    for (auto e : enumerate(array)) {
        std::cout << e.index << ": " << e.element << '\n';
    }

    std::cout << "initializer list\n";
    for (auto e : enumerate({0, 1, 4, 9, 16, 25})) {
        std::cout << e.index << "^2 = " << e.element << '\n';
    }

    std::cout << "range(10, 20, 2)\n";
    for (auto e : enumerate(range(10, 20, 2))) {
        std::cout << e.index << ": " << e.element << '\n';
    }

    std::cout << "range(10, 20, 2)\n";
    for (auto e : enumerate(enumerate(range(10, 20, 2)))) {
        std::cout << e.index << ": " << e.element.element << '\n';
    }

    std::cout << "vector temporary\n";
    for (auto e : enumerate(std::vector<int>(5,2))) {
        std::cout << e.index << ": " << e.element << '\n';
    }

    return 0;
}
