#include <imap.hpp>

#include <vector>
#include <iostream>

int main() {
    // applies the function on its way through the sequence
    std::cout << "mapping i*i over [1, 7): ";
    std::vector<int> vec1 = {1, 2, 3, 4, 5, 6};
    for (auto i : iter::imap([] (int i) {return i * i; }, vec1)) {
        std::cout << i << ' ';
    }
    std::cout << '\n';

    // multiple sequences can be used, each iterator will be advanced
    // f(a[0], b[0]) then f(a[1], b[1]) etc.
    std::cout << "vector sum of <1, 2, ..., 6> and <10, 20, ..., 60>: ";
    std::vector<int> vec2 = {10, 20, 30, 40, 50, 60};
    for (auto i : iter::imap([] (int x, int y) { return x + y; }, vec1, vec2)) {
        std::cout << i << ' ';
    }
    std::cout << '\n';

    // it will terminate on the shortest sequence
    std::cout << "vector sum of <1, 2, ..., 6>, <10, 20, ..., 60>, and "
        "<100, 200, 300>: ";
    std::vector<int> vec3 = {100, 200, 300};
    for (auto i : iter::imap([] (int a, int b, int c) { return a + b + c; }, 
                vec1, vec2, vec3)) {
        std::cout << i << ' ';
    }
    std::cout << '\n';
}
