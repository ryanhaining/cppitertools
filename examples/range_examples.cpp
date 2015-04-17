#include <range.hpp>

#include <iostream>

int main() {
    std::cout << "range(10): { ";
    for (auto i : iter::range(10)) {
        std::cout << i << ' ';
    }
    std::cout << "}\n";

    // range works with a start and exclusive stop
    std::cout << "range(20, 30): { ";
    for (auto i : iter::range(20, 30)) {
        std::cout << i << ' ';
    }
    std::cout << "}\n";

    // range supports a step size (prints 50, 52 ... 58)
    std::cout << "range(50, 60, 2): { ";
    for (auto i : iter::range(50, 60, 2)) {
        std::cout << i << ' ';
    }
    std::cout << "}\n";

    // ranges can cover negative values
    std::cout << "range(-10, 10, 2): { ";
    for (auto i : iter::range(-10, 10, 2)) {
        std::cout << i << ' ';
    }
    std::cout << "}\n";

    // the step size doesn't need to evenly divide the distance
    std::cout << "range(0, 5, 4): { ";
    for (auto i : iter::range(0, 5, 4)) {
        std::cout << i << ' ';
    }
    std::cout << "}\n";

    // ranges can go down with a negative step
    std::cout << "range(-1, -10, -2): { ";
    for (auto i : iter::range(-1, -10, -2)) {
        std::cout << i << ' ';
    }
    std::cout << "}\n";

    // ranges can work with floats and other types that act like numbers
    // the normal concerns with comparing floats still hold here
    std::cout << "range(5.0, 9.9, 0.5): { ";
    for (auto i : iter::range(5.0, 9.9, 0.5)) {
        std::cout << i << ' ';
    }
    std::cout << "}\n";

    // ranges of course support unsigned values
    std::cout << "range(10u): { ";
    for (auto i : iter::range(10u)) {
        std::cout << i << ' ';
    }
    std::cout << "}\n";
}
