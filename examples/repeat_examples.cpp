#include <repeat.hpp>

#include <iostream>

int main () {
    int a = 10;
    int i = 0;

    std::cout << "repeat(10) breaks after 20: ";
    for (auto&& num : iter::repeat(a)) { 
        std::cout << num << ' ';
        ++i;
        if (i >= 20) break;
    }
    std::cout << '\n';

    std::cout << "repeat(" << a << ", 15): repeats 15 times: ";
    for (auto&& num : iter::repeat(a,15)) {
        std::cout << num << ' ';
    }
    std::cout << '\n';
}
