#include <combinations_with_replacement.hpp>

#include <vector>
#include <string>
#include <iostream>
#include <array>

using iter::combinations_with_replacement;

int main() {
    int j = 0;
    std::vector<int> v = {1,2,3,4,5};
    for (auto i : combinations_with_replacement<4>(v)) {
        //std::cout << i << std::endl;
        for (auto j : i ) std::cout << *j << " ";
        std::cout<<std::endl;
        ++j;
        //if (j == 40) break;
    }
    return 0;
}
