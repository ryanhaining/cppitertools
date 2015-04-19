#include <dropwhile.hpp>

#include <vector>
#include <iostream>


int main() {
    std::vector<int> ns = {0, 1, 2, 3, 4, 5};
    std::cout << "ns = { ";
    for (auto&& i : ns) {
        std::cout << i << ' ';
    }
    std::cout << "}\n";

    std::cout << "dropwhile elements are less than 5\n";
    for (auto&& i : iter::dropwhile([] (int i) {return i < 5;}, ns)) {
        std::cout << i << '\n';
    }
}
