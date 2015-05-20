#include <zip_longest.hpp>

#include <vector>
#include <string>
#include <iostream>
#include <array>
#include <boost/optional.hpp>

// prints Just VALUE or Nothing
template <typename T>
std::ostream & operator<<(std::ostream & out, const boost::optional<T>& opt) {
    if (opt) {
        out << "Just " << *opt;
    } else {
        out << "Nothing";
    }
    return out;
}

int main() {
    std::vector<int> ivec = {1, 4, 9, 16, 25, 36};
    std::vector<std::string> svec = {"hello", "good day", "goodbye"};

    std::cout << "zipping a vector of strings with a vector of ints:\n";
    for (auto&& e : iter::zip_longest(ivec, svec)) {
        std::cout << '(' << std::get<0>(e) << ", "
            << std::get<1>(e)  << ")\n";
    }
}
