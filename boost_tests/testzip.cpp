#include "../zip.hpp"
#include "../chain.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <array>

#define BOOST_TEST_MODULE ZipTest test

#ifdef COMPILED_BINARY
    #include <boost/test/unit_test.hpp>
#else
    #include <boost/test/included/unit_test.hpp>
#endif

#include <boost/test/output_test_stream.hpp>
using boost::test_tools::output_test_stream;


using iter::zip;

BOOST_AUTO_TEST_CASE ( zip_test ) {

    output_test_stream output("pattern_files/zip_output.txt",true);
    //Ryan's test
    {
        std::vector<int> ivec{1, 4, 9, 16, 25, 36};
        std::vector<std::string> svec{"hello", "good day", "goodbye"};

        for (auto e : zip(ivec, svec)) {
            auto &i = std::get<0>(e);
            output << i << std::endl;
            i = 69;
            output << std::get<1>(e) << std::endl;
        }
        BOOST_REQUIRE(output.match_pattern());
        for (auto e : zip(ivec, svec)) {
            output << std::get<0>(e) << std::endl;
            output << std::get<1>(e) << std::endl;
        }
        BOOST_REQUIRE(output.match_pattern());
    }
    //Aaron's test
    {
        std::array<int,4> i{{1,2,3,4}};
        std::vector<float> f{1.2,1.4,12.3,4.5,9.9};
        std::vector<std::string> s{"i","like","apples","alot","dude"};
        std::array<double,5> d{{1.2,1.2,1.2,1.2,1.2}};
        output << std::endl << "Variadic template zip iterator" << std::endl;
        for (auto e : iter::zip(i,f,s,d)) {
            output << std::get<0>(e) << " " 
                << std::get<1>(e) << " " 
                << std::get<2>(e) << " "
                << std::get<3>(e) << std::endl;
            std::get<1>(e)=2.2f; //modify the float array
        }
        output<<std::endl;
        BOOST_REQUIRE(output.match_pattern());
        for (auto e : iter::zip(i,s,f,d)) {
            output << std::get<0>(e) << " " 
                << std::get<1>(e) << " "
                << std::get<2>(e) << " " 
                << std::get<3>(e) << std::endl;
        }
        BOOST_REQUIRE(output.match_pattern());
        output << std::endl << "Try some weird range differences" << std::endl;
        std::vector<int> empty{};
        for (auto e : iter::zip(empty,f,s,d)) {
            output << std::get<0>(e) << " " 
                << std::get<1>(e) << " " 
                << std::get<2>(e) << " "
                << std::get<3>(e) << std::endl;
        }
        output<<std::endl;
        BOOST_REQUIRE(output.match_pattern());
        for (auto e : iter::zip(f,s,empty,d)) {
            output << std::get<0>(e) << " " 
                << std::get<1>(e) << " " 
                << std::get<2>(e) << " "
                << std::get<3>(e) << std::endl;
        }//both should print nothing
        output<<std::endl;
        BOOST_REQUIRE(output.match_pattern());
        for (auto e : iter::zip(f,s,i,d)) { //i should be the limiter now
            output << std::get<0>(e) << " " 
                << std::get<1>(e) << " " 
                << std::get<2>(e) << " "
                << std::get<3>(e) << std::endl;
        }
        output<<std::endl;
        BOOST_REQUIRE(output.match_pattern());
        const std::vector<double> constvector{1.1,2.2,3.3,4.4};
        for (auto e : zip(iter::chain(std::vector<int>{1,5},std::array<int,2>{{1,2}}),
                    std::initializer_list<std::string>{"asdfas","aaron","ryan","apple","juice"},
                    constvector)) 
        { 

            output << std::get<0>(e) << " " 
                << std::get<1>(e) << " " 
                << std::get<2>(e) << std::endl;
        }
        BOOST_REQUIRE(output.match_pattern());
    }
    BOOST_REQUIRE(output.match_pattern());
}

