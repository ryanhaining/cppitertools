#include <combinations_with_replacement.hpp>

#include <vector>
#include <string>
#include <iostream>
#include <array>

using iter::combinations_with_replacement;

int main() {
    std::vector<int> v = {1,2,3,4,5};
    for (auto i : combinations_with_replacement<40>(v)) {
        //std::cout << i << std::endl;
        for (auto j : i ) std::cout << j << " ";
        std::cout<<std::endl;
    }
    return 0;
}
