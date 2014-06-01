#include <chain.hpp>
#include <range.hpp>

#include <vector>
#include <string>
#include <iostream>
#include <array>
#include <initializer_list>

using iter::chain;
using il = std::initializer_list<int>; 

int main() {
    {
        std::vector<int> ivec{1, 4, 7, 9};
        std::vector<int> lvec{100, 200, 300, 400, 500, 600};

        for (auto e : chain(ivec, lvec)) {
            std::cout << e << std::endl;
        }
    }
    {
        std::vector<int> empty{};
        std::vector<int> vec1{1,2,3,4,5,6};
        std::array<int,4> arr1{{7,8,9,10}};
        std::array<int,3> arr2{{11,12,13}};
        std::cout << std::endl << "Chain iter test" << std::endl;
        for (auto i : iter::chain(empty,vec1,arr1)) {
            std::cout << i << std::endl;
        }
        std::cout<<std::endl;
        for (auto i : iter::chain(vec1,empty,arr1)) {
            std::cout << i << std::endl;
        }
        std::cout<<std::endl;
        for (auto i : iter::chain(vec1,arr1,empty)) {
            std::cout << i << std::endl;
        }
        std::cout<<std::endl;//modifying the range
        for (auto & i : iter::chain(vec1,arr1,arr2)) {
            i = 0;//0 out the whole thing
        }
        std::cout<<std::endl;
        for (auto i : iter::chain(vec1,arr1,arr2)) {
            std::cout << i << std::endl;
        }
        //test only works with perfect forwarding
        std::cout<<std::endl;
        for (auto i : chain(il{1,2,3,4,5},il{6,7,8,9},il{10,11,12})) {
            std::cout << i << std::endl;
        }

        // test with temporaries
        std::cout << '\n';
        for (auto i : chain(std::vector<int>{1,2,3,4},
                    std::array<int,4>{{5,6,7,8}})) {
            std::cout << i << '\n';
        }
    }
    return 0;
}
