#include <zip.hpp>
#include <chain.hpp>
#include <range.hpp>
#include <iterbase.hpp>

#include <vector>
#include <string>
#include <iostream>
#include <array>
#include <list>
#include <forward_list>

using iter::zip;

using std::get;

int main() {
    //Ryan's test
    {
        std::vector<int> ivec{1, 4, 9, 16, 25, 36};
        std::vector<std::string> svec{"hello", "good day", "goodbye"};

		int count = 0;
        for (auto e : zip(ivec, svec)) {
            auto &i = get<0>(e);
            std::cout << i << std::endl;
            i = 69;
            std::cout << get<1>(e) << std::endl;
			if (count++ > 5){
			  exit(0);
			}
        }

        for (auto e : zip(ivec, svec)) {
            std::cout << get<0>(e) << std::endl;
            std::cout << get<1>(e) << std::endl;
        }

        for (auto e : zip(iter::range(10), iter::range(10, 20))) {
            std::cout << get<0>(e) << '\n';
            std::cout << get<1>(e) << '\n';
        }
    }
    //Aaron's test
    {
        std::array<int,4> i{{1,2,3,4}};
        std::vector<float> f{1.2,1.4,12.3,4.5,9.9};
        std::vector<std::string> s{"i","like","apples","alot","dude"};
        std::array<double,5> d{{1.2,1.2,1.2,1.2,1.2}};
        std::cout << std::endl << "Variadic template zip iterator" << std::endl;
        for (auto e : iter::zip(i,f,s,d)) {
            std::cout << get<0>(e) << " " 
                << get<1>(e) << " " 
                << get<2>(e) << " "
                << get<3>(e) << std::endl;
            get<1>(e)=2.2f; //modify the float array
        }
        std::cout<<std::endl;
        for (auto e : iter::zip(i,s,f,d)) {
            std::cout << get<0>(e) << " " 
                << get<1>(e) << " "
                << get<2>(e) << " " 
                << get<3>(e) << std::endl;
        }
        std::cout << std::endl << "Try some weird range differences" << std::endl;
        std::vector<int> empty{};
        for (auto e : iter::zip(empty,f,s,d)) {
            std::cout << get<0>(e) << " " 
                << get<1>(e) << " " 
                << get<2>(e) << " "
                << get<3>(e) << std::endl;
        }
        std::cout<<std::endl;
        for (auto e : iter::zip(f,s,empty,d)) {
            std::cout << get<0>(e) << " " 
                << get<1>(e) << " " 
                << get<2>(e) << " "
                << get<3>(e) << std::endl;
        }//both should print nothing
        std::cout<<std::endl;
        for (auto e : iter::zip(f,s,i,d)) { //i should be the limiter now
            std::cout << get<0>(e) << " " 
                << get<1>(e) << " " 
                << get<2>(e) << " "
                << get<3>(e) << std::endl;
        }
        std::cout<<std::endl;
        const std::vector<double> constvector{1.1,2.2,3.3,4.4};
        for (auto e : zip(iter::chain(std::vector<int>(5,5),std::array<int,2>{{1,2}}),
                    std::initializer_list<std::string>{"asdfas","aaron","ryan","apple","juice"},
                    constvector)) 
        { 

            std::cout << get<0>(e) << " " 
                << get<1>(e) << " " 
                << get<2>(e) << std::endl;
        }
    }
	
	{ //ben's test

	  std::vector<std::string> strings{"hello", "variadic", "template", "world"};
	  std::vector<int> intRange{0,1,2,3};
	  auto zipped = iter::zip(strings, intRange);
	  auto it = zipped.begin();
	  std::cout << get<0>(*it) << ' ' << get<1>(*it) << std::endl;
	  ++it;
	  std::cout << get<0>(*it) << ' ' << get<1>(*it) << std::endl;
	  it++;
	  std::cout << get<0>(*it) << ' ' << get<1>(*it) << std::endl;
	  --it;
	  std::cout << get<0>(*it) << ' ' << get<1>(*it) << std::endl;
	  it--;
	  std::cout << get<0>(*it) << ' ' << get<1>(*it) << std::endl;

	  std::sort(zipped.begin(), zipped.end(),
				[](decltype(zipped.begin())::value_type t,
				   decltype(zipped.begin())::value_type u){
				  return get<0>(t) < get<0>(u);
				});

	  std::cout << "sorted by string, in lockstep" << std::endl;
	  for(auto e : zipped){
		std::cout << get<0>(e) << ' ' << get<1>(e) << std::endl;
	  }

	}
    return 0;
}

