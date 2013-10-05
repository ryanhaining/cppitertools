CXX := clang++
CXXFLAGS := -Wall -Wextra -pedantic -std=c++11

KERNEL := $(shell uname -s)
ifeq ($(KERNEL),Darwin)
ifeq ($(CXX),clang++)
	CXXFLAGS += -stdlib=libc++
endif
endif

LINK.o = $(LINK.cc)

CPPFLAGS := -I".."

TESTS = \
		testchain \
		testcycle \
		testenumerate \
		testrange \
		testzip \
		testslice \
		testreverse \
		testfilter \
		testrepeat \
		testtakewhile \
		testdropwhile \
		testzip_longest \
		testproduct \
		testpermutations \
		testcompress \
		testcombinations_with_replacement \
		testcombinations \
		testpowerset \
		testmoving_section \
		testimap 

all: $(TESTS)

clean:
	rm -f *.o $(TESTS)
