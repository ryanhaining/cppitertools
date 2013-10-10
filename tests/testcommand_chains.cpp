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
    return 0;
}
