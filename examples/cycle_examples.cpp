#include <cycle.hpp>

#include <vector>
#include <string>
#include <iostream>

int main() {
    size_t count = 0;

    std::cout << "cycle({2, 4, 6}) run 20 times:\n";
    std::vector<int> vec = {2, 4, 6};
    for (auto&& i : iter::cycle(vec)) {
        std::cout << i << '\n';
        if (count == 20) {
            break;
        }
        ++count;
    }

    std::cout << "cycle(\"hello\") run 20 times:\n";
    count = 0;
    const std::string s("hello");
    for (auto&& c : iter::cycle(s)) {
        std::cout << c << '\n';
        if (count == 20) {
            break;
        }
        ++count;
    }
}
