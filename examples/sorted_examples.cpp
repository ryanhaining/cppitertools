#include <sorted.hpp>

#include <iostream>
#include <vector>
#include <unordered_set>
#include <string>

int main() {
    std::vector<int> vec = {19, 45, 32, 10, 0, 90, 15, 1, 7, 5, 6, 69};
    std::cout << "sorted(vector): ";
    for (auto&& i : iter::sorted(vec)) {
        std::cout << i << ' ';
    }
    std::cout << '\n';

    std::cout << "sorted by last character: ";
    std::vector<std::string> svec = {"hello", "everyone", "thanks", "for",
                                     "having", "me", "here", "today"};
    for (auto&& s : iter::sorted(svec,
                [] (const std::string& s1, const std::string& s2) {
                    return *s1.rbegin() < *s2.rbegin();})) {
        std::cout << s << ' ';
    }
    std::cout << '\n';

    // works even if the container isn't sortable
    std::cout << "unordered_set<int> sorted: ";
    std::unordered_set<int> uset = {10, 1, 20, 4, 50, 3};
    for (auto&& i : iter::sorted(uset)) {
        std::cout << i << ' ';
    }
    std::cout << '\n';
}
