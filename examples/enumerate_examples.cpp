#include <enumerate.hpp>

#include <string>
#include <iostream>
#include <vector>
#include <cassert>


int main() {
    std::cout << "enumerating the characters of a string \"hello\":\n";
    const std::string const_string("hello");
    for (auto&& e : iter::enumerate(const_string)) {
        std::cout << '(' << e.index << ", " << e.element << ") ";
    }
    std::cout << '\n';

    std::vector<int> vec = {20, 30, 50};
    std::cout << "enumerating a vector of {20, 30, 50}:\n";
    for (auto&& e : iter::enumerate(vec)) { 
        std::cout << '(' << e.index << ", " << e.element << ") ";
        e.element = 0;
    }
    std::cout << '\n';
    assert(vec[0] == 0);
    assert(vec[1] == 0);
    assert(vec[2] == 0);

    // itertools supports raw arrays
    std::cout << "statically sized arrays can be enumerated\n";
    int array[] = {1, 9, 8, 11};
    for (auto&& e : iter::enumerate(array)) {
        std::cout << '(' << e.index << ", " << e.element << ") ";
    }
    std::cout << '\n';

    // itertools supports temporaries
    std::cout << "vector temporary of {5, 2}\n";
    for (auto&& e : iter::enumerate(std::vector<int>(5,2))) {
        std::cout << '(' << e.index << ", " << e.element << ") ";
    }
    std::cout << '\n';
}
