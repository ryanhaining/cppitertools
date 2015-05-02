#include <range.hpp>

#include <iostream>

using iter::range;

int main()
{
    for (auto i : range(10)) {
        std::cout << i << std::endl;
    }
    for (auto i : range(20, 30)) {
        std::cout << i << std::endl;
    }
    for (auto i : range(50, 60, 2)) {
        std::cout << i << std::endl;
    }

    std::cout << "Negative Tests\n";
    for (auto i: range(-10, 0)) {
        std::cout << i << std::endl;
    }

    for (auto i : range(-10, 10, 2)) {
        std::cout << i << std::endl;
    }

    std::cout << "Tests where (stop - start)%step != 0" << std::endl;
    for (auto i : range(1, 10, 2)) {
        std::cout << i << std::endl;
    }

    for (auto i : range(-1, -10, -2)) {
        std::cout << i << std::endl;
    }
    
    std::cout << "Tests with different types" << std::endl;
    for(auto i : range(5.0, 10.0, 0.5)) {
        std::cout << i << std::endl;
    }
	std::cout << "test unsigned" << std::endl;
	std::cout << "empty range: " << std::endl;
	size_t len = 0;
	for(auto i : range(len)){
	  std::cout << i << std::endl;
	}
	std::cout << "stop only" << std::endl;
	len = 3;
	for(auto i : range(len)){
	  std::cout << i << std::endl;
	}
	std::cout << "start stop" << std::endl;
	size_t start = 1;
	for(auto i : range(start, len)){
	  std::cout << i << std::endl;
	}

	std::cout << "start stop skip" << std::endl;
	len = 10;
	size_t skip = 3;
	for(auto i : range(start, len, skip)){
	  std::cout << i << std::endl;
	}
	


    // invalid ranges:
    std::cout << "Should not print anything after this line until exception\n";
    for (auto i : range(-10, 0, -1)) {
        std::cout << i << std::endl;
    }

    for (auto i : range(0, 1, -1)) {
        std::cout << i << std::endl;
    }

    std::cout << "Should see exception now\n";
    for (auto i : range(0, 10, 0) ) {
        std::cout << i << std::endl;
    }

    return 0;
}
