CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++11 -fdiagnostics-color=auto
CPPFLAGS = -I..
LINK.o = $(CXX)

all: test_main

test_main: test_main.o test_range.o test_enumerate.o

test_range.o: test_range.cpp ../range.hpp
test_enumerate.o: test_enumerate.cpp ../enumerate.hpp

clean:
	rm -f test_main *.o
