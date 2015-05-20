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

    for (auto gb : groupby(vec, [] (const std::string &s) {return s.length(); })) {
        std::cout << "key: " << gb.first << '\n';
        std::cout << "content: ";
        for (auto s : gb.second) {
            std::cout << s << "  ";
        }
        std::cout << '\n';
    }

    std::cout << "skipping length of 3\n";
    for (auto gb : groupby(vec, &length)) {
        if (gb.first == 3) {
            continue;
        }
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

    for (auto gb : groupby("aabbccccdd", [] (const char c) {return c < 'c';})){
        std::cout << "key: " << gb.first << '\n';
        std::cout << "content: ";
        for (auto s : gb.second) {
            std::cout << s << "  ";
        }
        std::cout << '\n';
    }
    
    for (auto gb : groupby({'a', 'a', 'b', 'b', 'c'})) {
        std::cout << "key: " << gb.first << '\n';
        std::cout << "content: ";
        for (auto s : gb.second) {
            std::cout << s << "  ";
        }
        std::cout << '\n';
    }

    for (auto gb : groupby({'a', 'a', 'b', 'b', 'c', 'c', 'd', 'd'},
                [] (const char c) {return c < 'c'; })) {
        std::cout << "key: " << gb.first << '\n';
        std::cout << "content: ";
        for (auto s : gb.second) {
            std::cout << s << "  ";
        }
        std::cout << '\n';
    }

    std::cout << "with vector temporary:\n";
    for (auto gb : groupby(
                std::vector<char>{'a', 'a', 'b', 'b', 'c', 'c', 'd', 'd'},
                [] (const char c) {return c < 'c'; })) {
        std::cout << "key: " << gb.first << '\n';
        std::cout << "content: ";
        for (auto s : gb.second) {
            std::cout << s << "  ";
        }
        std::cout << '\n';
    }
        

    return 0;
}

    
