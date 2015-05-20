#include <iostream>

#include <slice.hpp>
#include <range.hpp>

#include <vector>
#include <array>

int main() {
    std::cout << std::endl << "Slice range test" << std::endl << std::endl;
    std::vector<int> a{0,1,2,3,4,5,6,7,8,9,10,11,12,13};
    std::vector<std::string> b{"hey","how","are","you","doing"};

    std::cout << "step out of slice\n";
    for (auto i : iter::slice(a, 1, 4, 5)) {
        std::cout << i << '\n';
    }
    std::cout << "end step out\n";

    for (auto i : iter::slice(a,2)) {
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

    std::cout<< "\n13 down to 0\n";
    for (auto i : iter::slice(a,13,-1,-1)) {
        std::cout << i << std::endl;
    }

    std::cout<< "\nunevent step [0:5:3]\n";
    for (auto i : iter::slice(a, 0, 5, 3)) {
        std::cout << i << '\n';
    }

    std::cout<< "\nInvalid range [1:10:-1]\n";
    for (auto i : iter::slice(a,1,10,-1)) {
        //invalid range returns two begin iters
        std::cout << i << std::endl;
    }
    std::cout<< "\nOversize range [1:100:1]\n";
    for (auto i : iter::slice(a,1,100,1)) {
        //invalid range returns two begin iters
        std::cout << i << std::endl;
    }
    std::cout<< "\nOversize range and undersize[1:100:1]\n";
    for (auto i : iter::slice(a,-100,100,1)) {
        //invalid range returns two begin iters
        std::cout << i << std::endl;
    }

    std::cout<< "\nstatic array[1:8:2]\n";
    int arr[10] = {0,1,2,3,4,5,6,7,8,9};
    for (auto i : iter::slice(arr,1,8,2)) {
        //invalid range returns two begin iters
        std::cout << i << std::endl;
    }

    std::cout << "\ninitializer list\n";
    for (auto i : iter::slice({1, 2, 4, 8, 16, 32, 64, 128}, 2, 6)) {
        std::cout << i << '\n';
    }

    std::cout << "\nvector temporary\n";
    for (auto i : iter::slice(
                std::vector<int>{1, 2, 4, 8, 16, 32, 64, 128}, 2, 6)) {
        std::cout << i << '\n';
    }

}
