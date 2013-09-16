CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++11

LINK.o = $(LINK.cc)

CPPFLAGS = -I".."

TESTS = \
		testchain \
		testcycle \
		testenumerate \
		testrange \
		testzip

all: $(TESTS)

clean:
	rm -f *.o $(TESTS)
