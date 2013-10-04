CXXFLAGS= -Wall -Wextra -pedantic -std=c++11 -I.. -I/usr/local/include
CXXFLAGS+= $$(sh ./is_mac.sh)
CPPFLAGS= -I.. -I/usr/local/include

include .depend

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
		testmoving_section


all: .depend $(TESTS)

.depend: /dev/null
	for a in *.cpp; do ($(CC) $(CPPFLAGS) -MM $$a >> .depend); done

clean:
	rm -f *.o $(TESTS) .depend
	touch .depend
# DO NOT DELETE
