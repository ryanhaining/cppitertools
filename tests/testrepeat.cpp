#include "repeat.hpp"
#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <memory>

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
    
    std::cout << "with temporary\n";
    for (auto s : iter::repeat(std::string{"hey"}, 2)) {
        std::cout <<  s << '\n';
    }

    std::cout << "with uptr temporary\n";
    for (auto& p : iter::repeat(std::unique_ptr<int>{new int{2}}, 2)) {
        std::cout << *p << '\n';
    }

}
