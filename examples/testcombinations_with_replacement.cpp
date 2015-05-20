#include "samples.hpp"
#include <combinations_with_replacement.hpp>
#include <range.hpp>

#include <vector>
#include <string>
#include <iostream>
#include <array>

using iter::combinations_with_replacement;

int main() {
    std::vector<itertest::MoveOnly> mv;
    for (auto i : iter::range(3)) {
        mv.emplace_back(i);
    }

    std::vector<int> v = {1,2,3,};
    for (auto i : combinations_with_replacement(v,4)) {
        for (auto j : i ) std::cout << j << " ";
        std::cout<<std::endl;
    }

    itertest::DerefByValue dbv;
    std::cout << "with deref by value iterator\n";
    for (auto i : combinations_with_replacement(dbv, 2)) {
        for (auto j : i ) std::cout << j << " ";
        std::cout<<std::endl;
    }

    std::cout << "with container of move-only\n";
    for (auto i : combinations_with_replacement(mv,2)) {
        for (auto j : i ) std::cout << j << " ";
        std::cout<<std::endl;
    }

    std::cout << "with temporary\n";
    for (auto i : combinations_with_replacement(std::vector<int>{1,2,3},4)) {
        for (auto j : i ) std::cout << j << " ";
        std::cout<<std::endl;
    }

    std::cout << "with init list\n";
    for (auto i : combinations_with_replacement({1,2,3},4)) {
        for (auto j : i ) std::cout << j << " ";
        std::cout<<std::endl;
    }

    std::cout << "with static array\n";
    int arr[] = {1, 2, 3};
    for (auto i : combinations_with_replacement(arr,4)) {
        for (auto j : i ) std::cout << j << " ";
        std::cout<<std::endl;
    }
}
