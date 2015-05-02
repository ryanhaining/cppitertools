#include <range.hpp>

#include <iostream>

int main() {
    // print [0, 10)
    std::cout << "range(10): { ";
    for (auto i : iter::range(10)) {
        std::cout << i << ' ';
    }
    std::cout << "}\n";

    // print [20, 30)
    std::cout << "range(20, 30): { ";
    for (auto i : iter::range(20, 30)) {
        std::cout << i << ' ';
    }
    std::cout << "}\n";

    // prints every second number in the range [50, 60)
    std::cout << "range(50, 60, 2): { ";
    for (auto i : iter::range(50, 60, 2)) {
        std::cout << i << ' ';
    }
    std::cout << "}\n";

    // prints every second number in the range [-10, 10)
    std::cout << "range(-10, 10, 2): { ";
    for (auto i : iter::range(-10, 10, 2)) {
        std::cout << i << ' ';
    }
    std::cout << "}\n";

    // the step doesn't need to cause i to equal stop eventually
    std::cout << "range(0, 5, 4): { ";
    for (auto i : iter::range(0, 5, 4)) {
        std::cout << i << ' ';
    }
    std::cout << "}\n";

    // ranges can count down as well as up
    std::cout << "range(-1, -10, -2): { ";
    for (auto i : iter::range(-1, -10, -2)) {
        std::cout << i << ' ';
    }
    std::cout << "}\n";

    // range works with floats and other types as well
    // the usual concerns with float comparison come into play here
    std::cout << "range(5.0, 9.9, 0.5): { ";
    for(auto i : iter::range(5.0, 9.9, 0.5)) {
        std::cout << i << ' ';
    }
    std::cout << "}\n";

    // range also works with unsigned values
    std::cout << "range(10u): { ";
    for(auto i : iter::range(10u)){
        std::cout << i << ' ';
    }
    std::cout << "}\n";
}
