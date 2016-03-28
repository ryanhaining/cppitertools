#include <starmap.hpp>

#include <array>
#include <tuple>
#include <vector>
#include <iostream>

// Since this is an example file I'm dumping a bunch of using declarations
// here, in real code I use using declarations very sparingly
using std::pair;
using std::tuple;
using std::vector;
using std::make_pair;
using std::make_tuple;

struct Callable {
    int operator()(int i) const { return i; }
    int operator()(int i, char c) const { return i + c; }
    int operator()(unsigned int u, int i, char c) const { return i + c + u; }
};

int main() {
    // the function will be called with the tuple-like object unpacked
    // using std::get.  This means an iterable of tuples, arrays, pairs, or
    // whatever works with std::get
    vector<pair<int, int>> v = {{2, 3}, {5, 2}, {3, 4}}; // {base, exponent}
    for (auto&& i : iter::starmap([](int b, int e){ return b * e; }, v)) {
        std::cout << i << '\n';
    }

    // Alternatively if an object has multiple call operators, a tuple-like
    // object of tuple-like objects 
    auto t = make_tuple(
            make_tuple(5), // first form
            make_pair(3, 'c'), // second form
            make_tuple(1u, 1, '1')); // third form
    for (auto&& i : iter::starmap(Callable{}, t)) {
        std::cout << i << '\n';
    }
}
