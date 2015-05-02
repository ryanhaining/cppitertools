#include <itertools.hpp>
#include <zip_longest.hpp>

#include <vector>
#include <tuple>
#include <array>
#include <iostream>
#include <string>

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
    {
        std::vector<int> vec1{1,2,3,4,5,6};
        std::vector<int> vec2{1,2,3,4,5};
        std::vector<std::string> strvec = 
            {"his","name","was","robert","paulson","his",
                "name","was","robert","paulson"};
        for (auto&& t : iter::zip_longest(iter::chain(vec1,vec2),strvec)) {
            std::cout << std::get<0>(t) << " "
                << std::get<1>(t) << std::endl;
        }
    }

    std::string str = "hello world";
    std::vector<int> vec = {6, 9, 6, 9};
    for (auto&& p : iter::enumerate(iter::enumerate(str))) { (void)p; }
    for (auto&& p : iter::enumerate(iter::zip(str, vec))) { (void)p; }

    std::cout << std::endl;
    {
        std::vector<int> vec1{1,2,3,4,5,6};
        std::vector<int> vec2{7,8,9,10};
        std::vector<std::string> strvec
        {"We're","done","when","I","say","we're","done"};
        for (auto&& t : iter::zip(strvec,
                    iter::chain(iter::slice(vec1,2,6),
                        iter::slice(vec2,1,4)))) {
            std::cout << std::get<0>(t) << " "
                << std::get<1>(t) << std::endl;
        }
    }
    std::cout << std::endl;
    {
        std::vector<int> vec1{1,2,3,4,5,6};
        std::vector<int> vec2{7,8,9,10};
        for (auto&& s : iter::sliding_window(iter::chain(vec1,vec2),4)) {
            for (auto&& i : s) std::cout << i << " ";
            std::cout<<std::endl;
        }
    }
    std::cout << std::endl;

    auto prod_range = iter::product(iter::range(10), iter::range(5));

    for (auto&& ij: iter::filter(
                [](std::tuple<unsigned, unsigned> const& c)
                {return std::get<0>(c) >= std::get<1>(c);},
                prod_range)) {
                    std::cout << std::get<0>(ij) << "," << std::get<1>(ij) << std::endl;
    }
}
