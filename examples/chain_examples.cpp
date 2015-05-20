#include <chain.hpp>

#include <vector>
#include <string>
#include <list>
#include <iostream>
#include <array>

int main() {
    // chaining combines two sequences for iteration
    std::vector<int> ivec = {1, 4, 7, 9};
    std::vector<int> lvec = {100, 200, 300, 400, 500, 600};
    std::cout << "chaining two int vectors together:\n";
    for (auto&& e : iter::chain(ivec, lvec)) {
        std::cout << e << ' ';
    }
    std::cout << '\n';

    std::vector<char> vec1 = {'c', 'h'};
    std::array<char,4> arr1{{'a', 'i', 'n', 'i'}};
    std::string s{"ng different "};
    std::list<char> lst = {'t', 'y', 'p', 'e', 's'};

    // chain can mix different sequence types as long as the type yielded
    // by their underlying iterators is *exactly* the same
    std::cout << "mixing: ";
    for (auto&& c : iter::chain(vec1, arr1, s, lst)) {
        std::cout << c;
    }
    std::cout << '\n';

    std::vector<std::vector<int>> matrix = {
        {2, 4, 6},
        {8, 10, 12},
        {14, 16, 18}
    };

    // chain.from_iterable effectively flattens a sequence by one level
    std::cout << "chain.from_iterable to flatten matrix:\n";
    for (auto&& i : iter::chain.from_iterable(matrix)) {
        std::cout << i << ' ';
    }
    std::cout << '\n';

}
