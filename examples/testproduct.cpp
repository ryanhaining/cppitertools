#include "samples.hpp"

#include <product.hpp>
#include <range.hpp>

#include <vector>
#include <iostream>
#include <string>

using iter::product;
int main() {

    std::vector<itertest::MoveOnly> mv;
    for (auto i : iter::range(10)) {
        mv.emplace_back(i);
    }
    std::vector<int> empty{};
    std::vector<int> v1{1,2,3};
    std::vector<int> v2{7,8};
    std::vector<std::string> v3{"the","cat"};
    std::vector<std::string> v4{"hi","what","up","dude"};

    for (auto t : product(v1, mv)) {
        std::cout << std::get<0>(t) << ", "
            << std::get<1>(t) << std::endl;
    }
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
    std::cout << '\n';

    for (auto t : product()) { t=t; }

    for (auto t : product(std::string{"hi"}, v1)) {
        std::cout << std::get<0>(t) << ", "
            << std::get<1>(t) << std::endl;
    }
    std::cout << '\n';

    int arr[] = {1,2};
    for (auto t : product(std::string{"hi"}, arr)) {
        std::cout << std::get<0>(t) << ", "
            << std::get<1>(t) << std::endl;
    }
    std::cout << '\n';
    for (auto&& ij: iter::product(iter::range(10), iter::range(5))) {
        std::cout << std::get<0>(ij) << "," << std::get<1>(ij) << std::endl;
    }

    return 0;
}
