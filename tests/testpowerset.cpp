#include "samples.hpp"
#include <powerset.hpp>
#include <range.hpp>
#include <vector>
#include <iostream>

using iter::powerset;

int main() {
    std::vector<int> vec {1,2,3,4,5,6,7,8,9};
    for (auto v : powerset(vec)) {
        for (auto i : v) std::cout << i << " ";
        std::cout << std::endl;
    }
    std::cout << "with temporary\n";
    for (auto v : powerset(std::vector<int>{1,2,3})) {
        for (auto i : v) std::cout << i << " ";
        std::cout << std::endl;
    }
    std::cout << "with initializer_list\n";
    for (auto v : powerset({1,2,3})) {
        for (auto i : v) std::cout << i << " ";
        std::cout << std::endl;
    }

    std::cout << "with container of move-only objects\n";
    std::vector<itertest::MoveOnly> mv;
    for (auto i : iter::range(3)) {
        mv.emplace_back(i);
    }
    for (auto v : powerset(mv)) {
        for (auto i : v) std::cout << i << " ";
        std::cout << std::endl;
    }

    std::cout << "with deref-by-value iterator\n";
    itertest::DerefByValue dbv;
    for (auto v : powerset(dbv)) {
        for (auto i : v) std::cout << i << " ";
        std::cout << std::endl;
    }

    return 0;
}
