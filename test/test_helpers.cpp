#include <utility>

#include "catch.hpp"
#include "helpers.hpp"

using itertest::SolidInt;
using itertest::IsIterator;
using itertest::IsMoveConstructibleOnly;

namespace {

  class ValidIter {
   private:
    int i;

   public:
    ValidIter& operator++();    // prefix
    ValidIter operator++(int);  // postfix
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

  REQUIRE(!IsIterator<InvalidIter>::value);
}

TEST_CASE("IsIterator fails when missing postfix ++", "[helpers]") {
  struct InvalidIter : ValidIter {
    InvalidIter operator++(int) = delete;
  };

  REQUIRE(!IsIterator<InvalidIter>::value);
}

TEST_CASE("IsIterator fails when missing ==", "[helpers]") {
  struct InvalidIter : ValidIter {
    bool operator==(const InvalidIter&) const = delete;
  };

  REQUIRE(!IsIterator<InvalidIter>::value);
}

TEST_CASE("IsIterator fails when missing !=", "[helpers]") {
  struct InvalidIter : ValidIter {
    bool operator!=(const InvalidIter&) const = delete;
  };

  REQUIRE(!IsIterator<InvalidIter>::value);
}

TEST_CASE("IsIterator fails when missing *", "[helpers]") {
  struct InvalidIter : ValidIter {
    int operator*() = delete;
  };

  REQUIRE(!IsIterator<InvalidIter>::value);
}

TEST_CASE("IsIterator fails when missing copy-ctor", "[helpers]") {
  struct InvalidIter : ValidIter {
    InvalidIter(const InvalidIter&) = delete;
  };

  REQUIRE(!IsIterator<InvalidIter>::value);
}

TEST_CASE("IsIterator fails when missing copy assignment", "[helpers]") {
  struct InvalidIter : ValidIter {
    InvalidIter& operator=(const InvalidIter&) = delete;
  };

  REQUIRE(!IsIterator<InvalidIter>::value);
}

TEST_CASE("IsIterator passes a valid iterator", "[helpers]") {
  REQUIRE(IsIterator<ValidIter>::value);
}

struct HasNothing {
  HasNothing(const HasNothing&) = delete;
  HasNothing& operator=(const HasNothing&) = delete;
};

struct HasMoveAndCopyCtor {
  HasMoveAndCopyCtor(const HasMoveAndCopyCtor&);
  HasMoveAndCopyCtor(HasMoveAndCopyCtor&&);
};

struct HasMoveCtorAndAssign {
  HasMoveCtorAndAssign(HasMoveCtorAndAssign&&);
  HasMoveCtorAndAssign& operator=(HasMoveCtorAndAssign&&);
};

struct HasMoveCtorAndCopyAssign {
  HasMoveCtorAndCopyAssign(HasMoveCtorAndCopyAssign&&);
  HasMoveCtorAndCopyAssign& operator=(const HasMoveCtorAndCopyAssign&);
};

struct HasMoveCtorOnly {
  HasMoveCtorOnly(HasMoveCtorOnly&&);
};

TEST_CASE("IsMoveConstructibleOnly false without move ctor", "[helpers]") {
  REQUIRE_FALSE(IsMoveConstructibleOnly<HasNothing>::value);
}

TEST_CASE("IsMoveConstructibleOnly false with copy ctor", "[helpers]") {
  REQUIRE_FALSE(IsMoveConstructibleOnly<HasMoveAndCopyCtor>::value);
}

TEST_CASE("IsMoveConstructibleOnly false with move assign", "[helpers]") {
  REQUIRE_FALSE(IsMoveConstructibleOnly<HasMoveCtorAndAssign>::value);
}
TEST_CASE("IsMoveConstructibleOnly false with copy assign", "[helpers]") {
  REQUIRE_FALSE(IsMoveConstructibleOnly<HasMoveCtorAndCopyAssign>::value);
}

TEST_CASE("IsMoveConstructibleOnly true when met", "[helpers]") {
  REQUIRE(IsMoveConstructibleOnly<HasMoveCtorOnly>::value);
}
