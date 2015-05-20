#include <reversed.hpp>

#include <iostream>
#include <vector>
#include <string>

int main () {
    std::vector<int> nums{1,2,3,4,5,6,7};
    std::vector<std::string> words{"hey","how","are","you","doing"};

    std::cout << "numbers reversed: ";
    for (auto&& i : iter::reversed(nums)) {
        std::cout << i << ' ';
    }
    std::cout << '\n';

    std::cout << "greeting reversed: ";
    for (auto&& s : iter::reversed(words)) {
        std::cout << s << ' ';
    }
    std::cout << '\n';

    std::cout << "statically sized array: ";
    int arr[] = {1, 2, 3, 4, 5, 6, 7};
    for (auto&& i : iter::reversed(arr)) {
        std::cout << i << ' ';
    }
    std::cout << '\n';
}
