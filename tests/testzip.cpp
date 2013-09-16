#include "zip.hpp"

#include <vector>
#include <string>
#include <iostream>

using iter::zip;

int main() {
    std::vector<int> ivec{1, 4, 9, 16, 25, 36};
    std::vector<std::string> svec{"hello", "good day", "goodbye"};

    for (auto e : zip(ivec, svec)) {
        auto &i = iter::zip_get<0>(e);
        std::cout << i << std::endl;
        i = 69;
        std::cout << iter::zip_get<1>(e) << std::endl;
    }

    for (auto e : zip(ivec, svec)) {
        std::cout << iter::zip_get<0>(e) << std::endl;
        std::cout << iter::zip_get<1>(e) << std::endl;
    }

    return 0;
}

