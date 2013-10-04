#include <imap.hpp>

#include <vector>
#include <iostream>

using iter::imap;

int main() {
    std::vector<int> vec= {1, 2, 3, 4, 5, 6};
    for (auto i : imap([] (int x) { return x * x; }, vec)) {
        std::cout << i << '\n';
    }
    return 0;
}
