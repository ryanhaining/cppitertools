#include "itertools.hpp"

#include <iostream>
#include <vector>
#include <array>
#include <string>

int main () {
    {
        std::vector<int> a{1,2,3,4,5,6,7};
        std::vector<std::string> b{"hey","how","are","you","doing"};
        std::cout << std::endl << "Reverse range test" << std::endl << std::endl;
        for (auto i : iter::reverse(a)) {
            std::cout << i << std::endl;
        }
        std::cout<<std::endl;
        //modify a
        for (auto & i : iter::reverse(a)) {
            std::cout << i << std::endl;
            i = 9;
        }
        std::cout<<std::endl;
        for (auto i : iter::reverse(a)) {
            std::cout << i << std::endl;
        }
        std::cout<<std::endl;
        //try strings
        for (auto s : iter::reverse(b)) {
            std::cout << s << std::endl;
        }
    }

    {
        std::cout << std::endl << "Slice range test" << std::endl << std::endl;
        std::vector<int> a{1,2,3,4,5,6,7};
        std::vector<std::string> b{"hey","how","are","you","doing"};
        for (auto i : iter::slice(a,0,2)) {
            std::cout << i << std::endl;
        }
        std::cout<<std::endl;
        for (auto & s : iter::slice(b,3,5)) {
            std::cout << s << std::endl;
            s = "test";
        }
        std::cout<<std::endl;
        for (auto s : b) {
            std::cout << s << std::endl;
        }
    }
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
    {
        std::vector<int> empty{};
        std::vector<int> vec1{1,2,3,4,5,6};
        std::array<int,4> arr1{{7,8,9,10}};
        std::array<int,3> arr2{{11,12,13}};
        std::cout << std::endl << "Chain iter test" << std::endl;
        for (auto i : iter::chain(empty,vec1,arr1)) {
            std::cout << i << std::endl;
        }
        std::cout<<std::endl;
        for (auto i : iter::chain(vec1,empty,arr1)) {
            std::cout << i << std::endl;
        }
        std::cout<<std::endl;
        for (auto i : iter::chain(vec1,arr1,empty)) {
            std::cout << i << std::endl;
        }
        std::cout<<std::endl;//modifying the range
        for (auto & i : iter::chain(vec1,arr1,arr2)) {
            i = 0;//0 out the whole thing
        }
        std::cout<<std::endl;
        for (auto i : iter::chain(vec1,arr1,arr2)) {
            std::cout << i << std::endl;
        }
    }
}
