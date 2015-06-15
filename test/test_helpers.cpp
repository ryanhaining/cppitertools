#include <utility>

#include "catch.hpp"
#include "helpers.hpp"

using itertest::SolidInt;
using itertest::IsIterator;

namespace {

class ValidIter {
    private:
        int i;
    public:
        ValidIter& operator++(); // prefix
        ValidIter operator++(int); // postfix
        bool operator==(const ValidIter&) const;
        bool operator!=(const ValidIter&) const;
        int operator*();
        void* operator->();
};

}

TEST_CASE("IsIterator fails when missing prefix ++", "[helpers]") {
    struct InvalidIter : ValidIter {
        InvalidIter& operator++() = delete;
    };

    REQUIRE( !IsIterator<InvalidIter>::value );
}

TEST_CASE("IsIterator fails when missing postfix ++", "[helpers]") {
    struct InvalidIter : ValidIter {
        InvalidIter operator++(int) = delete;
    };

    REQUIRE( !IsIterator<InvalidIter>::value );
}

TEST_CASE("IsIterator fails when missing ==", "[helpers]") {
    struct InvalidIter : ValidIter {
        bool operator==(const InvalidIter&) const = delete;
    };

    REQUIRE( !IsIterator<InvalidIter>::value );
}

TEST_CASE("IsIterator fails when missing !=", "[helpers]") {
    struct InvalidIter : ValidIter {
        bool operator!=(const InvalidIter&) const = delete;
    };

    REQUIRE( !IsIterator<InvalidIter>::value );
}

TEST_CASE("IsIterator fails when missing *", "[helpers]") {
    struct InvalidIter : ValidIter {
        int operator*() = delete;
    };

    REQUIRE( !IsIterator<InvalidIter>::value );
}

TEST_CASE("IsIterator fails when missing copy-ctor", "[helpers]") {
    struct InvalidIter : ValidIter {
        InvalidIter(const InvalidIter&) = delete;
    };

    REQUIRE( !IsIterator<InvalidIter>::value );
}

TEST_CASE("IsIterator fails when missing copy assignment", "[helpers]") {
    struct InvalidIter : ValidIter {
        InvalidIter& operator=(const InvalidIter&) = delete;
    };

    REQUIRE( !IsIterator<InvalidIter>::value );
}

TEST_CASE("IsIterator passes a valid iterator", "[helpers]") {
    REQUIRE( IsIterator<ValidIter>::value );
}
