#include <product.hpp>

#include <vector>
#include <iostream>
#include <string>

using iter::product;
//has trouble with empty ranges and more than 2 ranges
int main() {
    std::vector<int> empty{};
    std::vector<int> v1{1,2,3};
    std::vector<int> v2{7,8};
    std::vector<std::string> v3{"the","cat"};
    std::vector<std::string> v4{"hi","what","up","dude"};

    for (auto t : product(empty,v1)) {
        std::cout << std::get<0>(t) << ", "
            << std::get<1>(t) << std::endl;
    }
    for (auto t : product(v1,empty)) {
        std::cout << std::get<0>(t) << ", "
            << std::get<1>(t) << std::endl;
    }
    std::cout<<std::endl;
    for (auto t : product(v1,v2)) {
        std::cout << std::get<0>(t) << ", "
            << std::get<1>(t) << std::endl;
    }
    std::cout<<std::endl;
    for (auto t : product(v1,v2,v3,v4)) {
        std::cout << std::get<0>(t) << ", "
            << std::get<1>(t) << ", "
            << std::get<2>(t) << ", " 
            << std::get<3>(t) << std::endl;
    }
    std::cout<<std::endl;
    for (auto t : product(v1)) {
        std::cout << std::get<0>(t)  << std::endl;
    }
    std::cout<<std::endl;
    for (auto t : product(v1,v4)) {
        std::cout << std::get<0>(t) << ", "
          << std::get<1>(t)  << std::endl;
    }
    return 0;
}
