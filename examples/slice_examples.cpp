#include <iostream>

#include <slice.hpp>
#include <range.hpp>

#include <vector>
#include <array>

int main() {
    std::string a = "hello world";

    std::cout << "a = { ";
    for (auto&& i : a) {
        std::cout << i << ' ';
    }
    std::cout << "}\n";


    // just like range(), the step doesn't have to make it end anywhere
    // specific, below, the step is too great to cover more than two elements
    std::cout << "slice(a, 1, 4, 7): { ";
    for (auto&& i : iter::slice(a, 1, 4, 5)) {
        std::cout << i << ' ';
    }
    std::cout << "\n";

    std::cout << "slice(a, 2): { ";
    for (auto&& i : iter::slice(a, 2)) {
        std::cout << i << ' ';
    }
    std::cout << "}\n";
    
    std::cout << "slice(a, 1, 5): { ";
    for (auto&& i : iter::slice(a, 1, 5)) {
        std::cout << i << ' ';
    }
    std::cout << "}\n";

    std::cout << "slice(a, 2, 8, 2): { ";
    for (auto&& i : iter::slice(a, 2, 8, 2)) {
        std::cout << i << ' ';
    }
    std::cout << "}\n";
    
    // the stop can be beyond the end
    std::cout << "slice(a, 100): { ";
    for (auto&& i : iter::slice(a, 100)) {
        std::cout << i << ' ';
    }
    std::cout << "}\n";

    // the start can too
    std::cout << "slice(a, 100, 200): { ";
    for (auto&& i : iter::slice(a, 100, 200)) {
        std::cout << i << ' ';
    }
    std::cout << "}\n";
}
