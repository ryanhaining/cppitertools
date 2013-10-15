#include <sorted.hpp>

#include <iostream>
#include <vector>

using iter::sorted;

int main()
{
    std::vector<int> vec = {19, 3, 45, 32, 10, 0, 90, 15, 1, 7, 5, 6, 69};
    for (auto i : sorted(vec)) {
        std::cout << i << '\n';
    }

    const std::vector<int> cvec(vec);
    for (auto i : sorted(cvec)) {
        std::cout << i << '\n';
    }

    return 0;
}
