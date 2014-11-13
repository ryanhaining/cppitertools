CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++11
CPPFLAGS = -I..
LINK.o = $(CXX)

all: test_range

test_range: test_range.o run_test_range.o

test_range.o: test_range.cpp 

clean:
	rm -f test_range *.o
