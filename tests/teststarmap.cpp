#include <starmap.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <tuple>
#include <list>
#include <vector>
#include <array>

using iter::starmap;

double f(double d, int i) {
    return d * i;
}

std::string g(const std::string& s, int i, double d) {
    std::stringstream ss;
    ss << s << ' ' << i << ' ' << d;
    return ss.str();
}

void test_normal() {
    std::cout << "vector<pair<double, int>>\n";
    std::vector<std::pair<double, int>> v1 = {{1.0, 2}, {3.2, 42}, {6.9, 7}};
    for (auto&& i : starmap(f, v1)) {
        std::cout << i << '\n';
    }
    std::cout << '\n';

    std::cout << "list<type<string, int, double>\n";
    {
        using T = std::tuple<std::string, int, double>;
        std::list<T> li =
            {T{"hey", 42, 6.9}, T{"there", 3, 4.0}, T{"yall", 5, 3.1}};
        for (auto&& s : starmap(g, li)) {
            std::cout << s << '\n';
        }
    }
    std::cout << '\n';
}

struct Callable {
    int operator()(int a, int b, int c) {
        return a + b + c;
    }

    int operator()(int a) {
        return a;
    }
};

void test_tuple_of_tuples() {
    auto tup = std::make_tuple(std::make_tuple(10, 19, 60),std::make_tuple(7));
    Callable c;
    std::cout << "tuple<tuple<int, int, int>, tuple<int>>\n";
    for (auto&& i : starmap(c, tup)) {
        std::cout << i << '\n';
    }

    auto tup2 = std::make_tuple(std::array<int, 3>{15, 100, 2000},
                               std::make_tuple(16));
    std::cout << "tuple<array<int>, tuple<int>>\n";
    for (auto&& i : starmap(c, tup2)) {
        std::cout << i << '\n';
    }
    std::cout << '\n';
}

int main() {
    test_normal();
    test_tuple_of_tuples();

}
