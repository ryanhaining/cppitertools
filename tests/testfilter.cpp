#include <filter.hpp>
#include <range.hpp>

#include <vector>
#include <iostream>

using iter::filter;

bool greater_than_four(int i) {
    return i > 4;
}

class LessThanValue {
    private:
        int compare_val;

    public:
        LessThanValue() = delete;
        LessThanValue(int v) : compare_val(v) { }
        
        bool operator() (int i) {
            return i < this->compare_val;
        }
};


int main() {
    std::vector<int> vec{1, 5, 6, 7, 2, 3, 8, 3, 2, 1};

    std::cout << "Greater than 4 (function pointer)\n";
    for (auto i : filter(greater_than_four, vec)) {
        std::cout << i << '\n';
    }

    std::cout << "Less than 4 (lambda)\n";
    for (auto i : filter([] (const int i) { return i < 4; }, vec)) {
        std::cout << i << '\n';
    }

    LessThanValue lv(4);
    std::cout << "Less than 4 (callable object)\n";
    for (auto i : filter(lv, vec)) {
        std::cout << i << '\n';
    }

    std::cout << "Nonzero ints filter(vec2)\n";
    std::vector<int> vec2 {0, 1, 2, 0, 3, 0, 0, 0, 4, 5, 0};
    for (auto i : filter(vec2)) {
        std::cout << i << '\n';
    }

    std::cout << "odd numbers in range(10) temp\n";
    for (auto i : filter([] (const int i) {return i % 2;}, iter::range(10))) {
        std::cout << i << '\n';
    }

    std::cout << "range(-1, 2)\n";
    for (auto i : filter(iter::range(-1, 2))) {
        std::cout << i << '\n';
    }


    std::cout << "ever numbers in initializer_list\n";
    for (auto i : filter([] (const int i) {return i % 2 == 0;},
                         {1, 2, 3, 4, 5, 6, 7}))
    {
        std::cout << i << '\n';
    }

    std::cout << "default in initialization_list\n";
    for (auto i : filter({-2, -1, 0, 0, 0, 1, 2})) {
        std::cout << i << '\n';
    }

    std::cout << "ever numbers in vector temporary\n";
    for (auto i : filter([] (const int i) {return i % 2 == 0;},
                         std::vector<int>{1, 2, 3, 4, 5, 6, 7}))
    {
        std::cout << i << '\n';
    }

    return 0;
}
