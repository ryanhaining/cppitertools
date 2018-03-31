#include <enumerate.hpp>

#include <string>
#include <iostream>
#include <vector>
#include <cassert>


int main() {
    std::cout << "enumerating the characters of a string \"hello\":\n";
    const std::string const_string("hello");
    for (auto&& [i, c] : iter::enumerate(const_string)) {
        std::cout << '(' << i << ", " << c << ") ";
    }
    std::cout << '\n';

    std::vector<int> vec = {20, 30, 50};
    std::cout << "enumerating a vector of {20, 30, 50}:\n";
    for (auto&& [i, n] : iter::enumerate(vec)) { 
        std::cout << '(' << i << ", " << n << ") ";
        n = 0;
    }
    std::cout << '\n';
    assert(vec[0] == 0);
    assert(vec[1] == 0);
    assert(vec[2] == 0);

    // itertools supports raw arrays
    std::cout << "statically sized arrays can be enumerated\n";
    int array[] = {1, 9, 8, 11};
    for (auto&& [i, n]  : iter::enumerate(array)) {
        std::cout << '(' << i << ", " << n << ") ";
    }
    std::cout << '\n';

    // itertools supports temporaries
    std::cout << "vector temporary of {5, 2}\n";
    for (auto&& [i, n] : iter::enumerate(std::vector<int>(5,2))) {
        std::cout << '(' << i << ", " << n << ") ";
    }
    std::cout << '\n';
}
