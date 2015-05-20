#include <unique_everseen.hpp>

#include <vector>
#include <iostream>

int main() {
    std::vector<int> v {1,2,3,4,3,2,1,5,6,7,7,8,9,8,9,6};
    std::cout << "omits all duplicates: ";
    for (auto&& i : iter::unique_everseen(v)) {
        std::cout << i << ' ';
    }
    std::cout << '\n';
}
