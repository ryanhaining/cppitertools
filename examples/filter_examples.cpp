#include <filter.hpp>

#include <vector>
#include <iostream>

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
    std::vector<int> ns{1, 5, 6, 0, 7, 2, 3, 8, 3, 0, 2, 1};
    std::cout << "ns = { ";
    for (auto&& i : ns) {
        std::cout << i << ' ';
    }
    std::cout << "}\n";

    std::cout << "Greater than 4 (function pointer)\n";
    for (auto&& i : iter::filter(greater_than_four, ns)) {
        std::cout << i << '\n';
    }

    std::cout << "Less than 4 (lambda)\n";
    for (auto&& i : iter::filter([] (const int i) { return i < 4; }, ns)) {
        std::cout << i << '\n';
    }

    LessThanValue lv(4);
    std::cout << "Less than 4 (callable object)\n";
    for (auto&& i : iter::filter(lv, ns)) {
        std::cout << i << '\n';
    }

    // filter(seq) with no predicate uses the truthiness of the values
    std::cout << "Nonzero ints filter(ns)\n";
    for (auto&& i : iter::filter(ns)) {
        std::cout << i << '\n';
    }

    std::cout << "odd numbers\n";
    for (auto&& i : iter::filter([] (const int i) {return i % 2;}, ns)) {
        std::cout << i << '\n';
    }
}
