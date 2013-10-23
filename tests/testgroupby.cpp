#include <groupby.hpp>

#include <vector>
#include <iostream>
#include <string>

using iter::groupby;


int length(std::string s)
{
    return s.length();
}

int main()
{
    std::vector<std::string> vec = {
        "hi", "ab", "ho",
        "abc", "def",
        "abcde", "efghi"
    };

    for (auto gb : groupby(vec, &length)) {
        std::cout << "key: " << gb.first << '\n';
        std::cout << "content: ";
        for (auto s : gb.second) {
            std::cout << s << "  ";
        }
        std::cout << '\n';
    }

    std::vector<int> ivec = {5, 5, 6, 6, 19, 19, 19, 19, 69, 0, 10, 10};
    for (auto gb : groupby(ivec)) {
        std::cout << "key: " << gb.first << '\n';
        std::cout << "content: ";
        for (auto s : gb.second) {
            std::cout << s << "  ";
        }
        std::cout << '\n';
    }


    return 0;
}

    
