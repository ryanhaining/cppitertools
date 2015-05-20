#include <chain.hpp>

#include <iostream>
#include <vector>

using iter::chain;

int main() {
    std::vector<std::vector<int>> matrix = {
        {1, 2, 3},
        {4, 5},
        {6, 8, 9, 10, 11, 12}
    };
    for (auto i : chain.from_iterable(matrix)) {
        std::cout << i << '\n';
    }

    std::cout << "with temporary\n";
    for (auto i : chain.from_iterable(std::vector<std::vector<int>>{
                    {1, 2, 3},
                    {4, 5},
                    {6, 8, 9, 10, 11, 12}
                })) {
        std::cout << i << '\n';
    }

    return 0;
}
