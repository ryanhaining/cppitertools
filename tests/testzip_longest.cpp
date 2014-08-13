#include <zip_longest.hpp>

#include <vector>
#include <string>
#include <iostream>
#include <array>
#include <boost/optional.hpp>

using iter::zip_longest;

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
    //Ryan's test
    {
        std::vector<int> ivec{1, 4, 9, 16, 25, 36};
        std::vector<std::string> svec{"hello", "good day", "goodbye"};

        for (auto e : zip_longest(ivec, svec)) {
            std::cout << std::get<0>(e) << std::endl; 
            std::cout << std::get<1>(e) << std::endl; 
        }

        for (auto e : zip_longest("helloworld",
                    std::vector<int>{1,2,3})) {
            std::cout << std::get<0>(e) << std::endl; 
            std::cout << std::get<1>(e) << std::endl; 
        }
    }
    //Aaron's test
    {
        std::array<int,4> i{{1,2,3,4}};
        std::vector<float> f{1.2,1.4,12.3,4.5,9.9};
        std::vector<std::string> s{"i","like","apples","alot","dude"};
        std::array<double,5> d{{1.2,1.2,1.2,1.2,1.2}};
        std::cout << std::endl << "Variadic template zip_longest" << std::endl;
        for (auto e : iter::zip_longest(i,f,s,d)) {
            std::cout << std::get<0>(e) << ' '
                << std::get<1>(e) << ' '
                << std::get<2>(e) << ' '
                << std::get<3>(e) << std::endl;
            *std::get<1>(e)=2.2f; //modify the float array
        }
        std::cout<<"modified array" <<std::endl;
        for (auto e : iter::zip_longest(i,s,f,d)) {
            std::cout << std::get<0>(e) << ' '
                << std::get<1>(e) << ' '
                << std::get<2>(e) << ' '
                << std::get<3>(e) << std::endl;
        }
        std::cout << std::endl << "Try some weird range differences" << std::endl;
        std::vector<int> empty{};
        for (auto e : iter::zip_longest(empty,f,s,d)) {
            std::cout << std::get<0>(e) << ' '
                << std::get<1>(e) << ' '
                << std::get<2>(e) << ' '
                << std::get<3>(e) << std::endl;
        }
        std::cout<<std::endl;
        for (auto e : iter::zip_longest(f,empty,s,d)) {
            std::cout << std::get<0>(e) << ' '
                << std::get<1>(e) << ' '
                << std::get<2>(e) << ' '
                << std::get<3>(e) << std::endl;
        }
        std::cout<<std::endl;
        for (auto e : iter::zip_longest(f,s,i,d)) { 
            std::cout << std::get<0>(e) << ' '
                << std::get<1>(e) << ' '
                << std::get<2>(e) << ' '
                << std::get<3>(e) << std::endl;
        }
        std::cout<<std::endl;
        for (auto e : iter::zip_longest(std::initializer_list<int>{1,2,3,4,5,6},
                    std::initializer_list<double>{1.1,2.2,3.3,4.4},
                    std::initializer_list<double>{1.1,2.2,3.3,4.4},
                    std::array<int,3>{{1,2,3}})) { 
            std::cout << std::get<0>(e) << ' '
                << std::get<1>(e) << ' '
                << std::get<2>(e) << ' '
                << std::get<3>(e) << std::endl;
        }
        std::cout<<std::endl;

    }
    return 0;
}

