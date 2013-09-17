#include <slice.hpp>

#include <iostream>
#include <vector>
#include <array>

int main() {
    std::cout << std::endl << "Slice range test" << std::endl << std::endl;
    std::vector<int> a{0,1,2,3,4,5,6,7,8,9,10,11,12,13};
    std::vector<std::string> b{"hey","how","are","you","doing"};
    for (auto i : iter::slice(a,0,2)) {
        std::cout << i << std::endl;
    }
    std::cout<<std::endl;
    for (auto & s : iter::slice(b,3,4)) {
        std::cout << s << std::endl;
        s = "test";
    }
    std::cout<<std::endl;
    for (auto s : b) {
        std::cout << s << std::endl;
    }
    std::cout<<std::endl;
    for (auto i : iter::slice(a,0,15,3)) {
        std::cout << i << std::endl;
    }
    std::cout<<std::endl;
    for (auto i : iter::slice(a,13,-1,-1)) {
        std::cout << i << std::endl;
    }
}
