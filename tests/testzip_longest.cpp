#include <zip_longest.hpp>

#include <vector>
#include <string>
#include <iostream>
#include <array>
#include <boost/optional.hpp>

using iter::zip_longest;

template <typename T>
std::ostream & operator<<(std::ostream & o, const boost::optional<T> & opt) {
    if (opt) {
        std::cout << **opt << std::endl;
    }
    else {
        std::cout << "Object disengaged" << std::endl;
    }
    return o;
}

int main() {
    //Ryan's test
    {
        std::vector<int> ivec{1, 4, 9, 16, 25, 36};
        std::vector<std::string> svec{"hello", "good day", "goodbye"};

        for (auto e : zip_longest(ivec, svec)) {
            std::cout << std::get<0>(e) << std::endl; 
            //has to deref iter and the optional object
            std::cout << std::get<1>(e) << std::endl; 
        }
    }
    /*
    //Aaron's test
    {
        std::array<int,4> i{{1,2,3,4}};
        std::vector<float> f{1.2,1.4,12.3,4.5,9.9};
        std::vector<std::string> s{"i","like","apples","alot","dude"};
        std::array<double,5> d{{1.2,1.2,1.2,1.2,1.2}};
        std::cout << std::endl << "Variadic template zip iterator" << std::endl;
        for (auto e : iter::zip(i,f,s,d)) {
            std::cout << iter::zip_get<0>(e) << " " 
                << iter::zip_get<1>(e) << " " 
                << iter::zip_get<2>(e) << " "
                << iter::zip_get<3>(e) << std::endl;
            iter::zip_get<1>(e)=2.2f; //modify the float array
        }
        std::cout<<std::endl;
        for (auto e : iter::zip(i,s,f,d)) {
            std::cout << iter::zip_get<0>(e) << " " 
                << iter::zip_get<1>(e) << " "
                << iter::zip_get<2>(e) << " " 
                << iter::zip_get<3>(e) << std::endl;
        }
        std::cout << std::endl << "Try some weird range differences" << std::endl;
        std::vector<int> empty{};
        for (auto e : iter::zip(empty,f,s,d)) {
            std::cout << iter::zip_get<0>(e) << " " 
                << iter::zip_get<1>(e) << " " 
                << iter::zip_get<2>(e) << " "
                << iter::zip_get<3>(e) << std::endl;
        }
        std::cout<<std::endl;
        for (auto e : iter::zip(f,s,empty,d)) {
            std::cout << iter::zip_get<0>(e) << " " 
                << iter::zip_get<1>(e) << " " 
                << iter::zip_get<2>(e) << " "
                << iter::zip_get<3>(e) << std::endl;
        }//both should print nothing
        std::cout<<std::endl;
        for (auto e : iter::zip(f,s,i,d)) { //i should be the limiter now
            std::cout << iter::zip_get<0>(e) << " " 
                << iter::zip_get<1>(e) << " " 
                << iter::zip_get<2>(e) << " "
                << iter::zip_get<3>(e) << std::endl;
        }
        std::cout<<std::endl;
    }
    */
    return 0;
}

