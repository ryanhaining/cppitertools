#include "repeat.hpp"
#include <iostream>
#include <vector>
#include <array>
#include <string>

int main () {
    int a = 10;
    int i = 0;
    for (auto num : iter::repeat(a)) {//goes infintely
        std::cout << num << std::endl;
        ++i;
        if (i > 20) break;
    }
    std::cout<<std::endl;
    for (auto num : iter::repeat(a,10)) {//goes ten times
        std::cout << num << std::endl;
    }
}
