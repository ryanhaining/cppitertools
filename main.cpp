#include <iostream>
#include <memory>
#include <string>
#include <itertools.hpp>


using namespace std::string_literals;

int main() {
    bool value = true;
    while(value)
        value += true;
    std::cout << value << std::endl;
}
