#include <imap.hpp>

#include <vector>
#include <iostream>

using iter::imap;

int main() {
    std::vector<int> vec1 = {1, 2, 3, 4, 5, 6};
    std::vector<int> vec2 = {10, 20, 30, 40, 50, 60};
    for (auto i : imap([] (int x, int y) { return x + y; }, vec1, vec2)) {
        std::cout << i << '\n';
    }
    return 0;
}
