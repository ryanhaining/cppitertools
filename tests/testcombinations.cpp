#include "samples.hpp"
#include <combinations.hpp>
#include <range.hpp>

#include <vector>
#include <string>
#include <iostream>
#include <array>

using iter::combinations;
int main() {
    itertest::DerefByValue dbv; 
    std::vector<itertest::MoveOnly> mv;
    for (auto i : iter::range(3)) {
        mv.emplace_back(i);
    }
    std::vector<int> v = {1,2,3,4,5};

    for (auto i : combinations(mv,2)) {
        for (auto j : i ) std::cout << j << " ";
        std::cout<<std::endl;
    }

    for (auto i : combinations(v,0)) {
        for (auto j : i ) std::cout << j << " ";
        std::cout<<std::endl;
    }

    for (auto i : combinations(v,3)) {
        for (auto j : i ) std::cout << j << " ";
        std::cout<<std::endl;
    }

    for (auto i : combinations(v,1)) {
        for (auto j : i ) std::cout << j << " ";
        std::cout<<std::endl;
    }

    for (auto i : combinations(v,5)) {
        for (auto j : i ) std::cout << j << " ";
        std::cout<<std::endl;
    }

    std::cout << "deref by value\n";
    for (auto i : combinations(dbv, 2)) {
        for (auto j : i) std::cout << j << " ";
        std::cout << '\n';
    }

    std::cout << "initializer list\n";
    for (auto i : combinations({1,2,3,4},3)) {
        for (auto j : i ) std::cout << j << " ";
        std::cout<<std::endl;

    }

    std::cout << "initializer list\n";
    for (auto i : combinations({1,2,3,4},1)) {
        for (auto j : i ) std::cout << j << " ";
        std::cout<<std::endl;
    }

    std::cout << "temporary\n";
    for (auto i : combinations(std::vector<int>{1,2,3,4,5}, 3)) {
        for (auto j : i ) std::cout << j << " ";
        std::cout<<std::endl;
    }

    std::cout << "static array\n";
    int array[] = {1,2,3,4,5};
    for (auto i : combinations(array, 3)) {
        for (auto j : i) std::cout << j << " ";
        std::cout << '\n';
    }
}
