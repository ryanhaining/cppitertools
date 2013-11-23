#include <itertools.hpp>

#include <vector>
#include <tuple>
#include <array>
#include <iostream>
#include <string>

using namespace iter;

template <typename T>
std::ostream & operator<<(std::ostream & o, const boost::optional<T> & opt) {
    if (opt) {
        std::cout << *opt; 
    }
    else {
        std::cout << "Object disengaged of type " << typeid(T).name();
    }
    return o;
}

int main() {
    {
        std::vector<int> vec1{1,2,3,4,5,6};
        std::vector<int> vec2{1,2,3,4,5};
        std::vector<std::string> strvec
        {"his","name","was","robert","paulson","his","name","was","robert","paulson"};
        for (auto t : zip_longest(chain(vec1,vec2),strvec)) {
            std::cout << std::get<0>(t) << " "
                << std::get<1>(t) << std::endl;
        }
    }
    std::cout << std::endl;
    /*
    {
        std::vector<int> vec1{1,2,3,4,5,6};
        std::vector<int> vec2{7,8,9,10};
        std::vector<std::string> strvec
        {"We're","done","when","I","say","we're","done"};
        for (auto t : zip(strvec,chain(slice(vec1,2,6),slice(vec2,1,4)))) {
            std::cout << std::get<0>(t) << " "
                << std::get<1>(t) << std::endl;
        }
    }
    */
    std::cout << std::endl;
    {
        std::vector<int> vec1{1,2,3,4,5,6};
        std::vector<int> vec2{7,8,9,10};
        for (auto s : moving_section(chain(vec1,vec2),4)) {
            for (auto i : s) std::cout << i << " ";
            std::cout<<std::endl;
        }
    }
    std::cout << std::endl;
    {
        std::vector<int> vec1{1,2,3,4,5,6};
        std::vector<int> vec2{7,8,9,10};
        for (auto s : grouper(chain(vec1,vec2),3)) {
            for (auto i : s) std::cout << i << " ";
            std::cout<<std::endl;
        }
    }
    return 0;
}
