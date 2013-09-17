CXX = clang++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++11

ifeq ($(CXX),clang++)
	CXXFLAGS += -stdlib=libc++
endif

LINK.o = $(LINK.cc)

CPPFLAGS = -I".."

TESTS = \
		testchain \
		testcycle \
		testenumerate \
		testrange \
		testzip \
		testslice \
		testreverse \
		testfilter

all: $(TESTS)

clean:
	rm -f *.o $(TESTS)
