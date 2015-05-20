#include <compress.hpp>

#include <vector>
#include <iostream>

template <typename DataType, typename SelectorType>
void testcase(std::vector<DataType> data_vec,
        std::vector<SelectorType> sel_vec)
{

    for (auto e : compress(data_vec, sel_vec)) {
        std::cout << e << '\n';
    }
}

int main(void) {
    using BVec = const std::vector<bool>;

    std::vector<int> ns{0, 1, 2, 3, 4, 5};
    std::cout << "ns = { ";
    for (auto&& i : ns) {
        std::cout << i << ' ';
    }
    std::cout << "}\n";

    BVec b1{true, false, true, false, true, false};

    std::cout << "compress(ns, {true, false, true, false, true, false}): { ";
    for (auto&& i : iter::compress(ns, b1)) {
        std::cout << i << ' ';
    }
    std::cout << "}\n";
    

    BVec b2 {true}; 
    // compress terminates on the shortest sequence (either one)
    std::cout << "compress(ns, {true}): { ";
    for (auto&& i : iter::compress(ns, b2)) {
        std::cout << i << ' ';
    }
    std::cout << "}\n";
    
}
