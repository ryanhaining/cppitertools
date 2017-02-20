// NOTE this header tests implementation details

#include "catch.hpp"
#include "internal/iterator_wrapper.hpp"

// I'm using a std::vector of 1 int instead of just an int in order to give
// the iterator types non-trivial constructors, destructors, and assignment.

// same begin() and end() types
struct SameTypes {
  struct iterator {
    iterator(int) : value_(1) {}

    bool operator!=(const iterator& other) const {
      return value_ != other.value_;
    }
    iterator& operator++() {
      ++value_.front();
      return *this;
    }
    const int& operator*() const {
      return value_.front();
    }
    std::vector<int> value_;  // non-trvial operations
  };

  iterator begin() const {
    return {0};
  }
  iterator end() const {
    return {0};
  }
};

// different begin() and end() types
struct DifferentTypes {
  struct iterator;
  struct end_iterator;
  struct iterator {
    iterator() : value_{} {
      REQUIRE(false);
    }
    iterator(int i) : value_(1, i) {}

    bool operator!=(const iterator& other) const {
      return value() != other.value();
    }
    bool operator!=(const end_iterator&) const {
      return value() != 3;
    }
    iterator& operator++() {
      ++value_.front();
      return *this;
    }
    const int& operator*() const {
      return value();
    }

    const int& value() const {
      return value_.front();
    }
    std::vector<int> value_;
  };

  struct end_iterator {
    end_iterator() {
      REQUIRE(false);
    }
    end_iterator(int) {}

    bool operator!=(const end_iterator&) const {
      return false;
    }
    bool operator!=(const iterator& other) const {
      return other.value() != 3;
    }
    end_iterator& operator++() {
      return *this;
    }
    const int& operator*() const {
      assert(false);
      return value();
    }

    const int& value() const {
      return value_.front();
    }
    std::vector<int> value_{};
  };

  iterator begin() const {
    return {0};
  }
  end_iterator end() const {
    return {0};
  }
};

// Explicit instatiations, which could cause failures if the implementation
// details of the implementation details change.
template class iter::impl::
    IteratorWrapperImpl<iter::impl::iterator_type<DifferentTypes>,
        iter::impl::iterator_end_type<DifferentTypes>>;

using iter::impl::IteratorWrapper;

TEST_CASE("ensure test type iterators are totally comparable", "[test_util") {
  {
    SameTypes s{};
    auto it = s.begin();
    (void)(it != it);
  }

  {
    DifferentTypes d{};
    auto b = d.begin();
    auto e = d.end();
    (void)(b != b);
    (void)(e != e);
    (void)(b != e);
    (void)(e != b);
  }
}

TEST_CASE(
    "Operations on IteratorWrappers with SameTypes work", "[base_iterator]") {
  SameTypes s;
  IteratorWrapper<SameTypes> it(s.begin());
  REQUIRE((std::is_same<std::decay_t<decltype(it)>,
      std::decay_t<decltype(s.begin())>>{}));
  REQUIRE(*it == 0);
  ++it;
  REQUIRE(*it == 1);
}

TEST_CASE("Operations on IteratorWrappers with DifferentTypes work",
    "[base_iterator]") {
  DifferentTypes d;
  using BI = IteratorWrapper<DifferentTypes>;
  BI it(d.begin());
  REQUIRE((!std::is_same<std::decay_t<decltype(it)>,
           std::decay_t<decltype(d.begin())>>{}));
  REQUIRE(*it == 0);
  ++it;
  REQUIRE(*it == 1);

  BI it2(d.begin());

  REQUIRE(it != it2);
  REQUIRE(it2 != it);
  ++it2;

  REQUIRE_FALSE(it != it2);
  REQUIRE_FALSE(it2 != it);

  BI bend(d.end());
  REQUIRE(it != bend);
  REQUIRE(bend != it);

  ++it;
  ++it;
  REQUIRE_FALSE(it != bend);
  REQUIRE_FALSE(bend != it);
}

TEST_CASE(
    "Can copy construct a IteratorWrapper with SameTypes", "[base_iterator]") {
  SameTypes s;
  using BI = IteratorWrapper<SameTypes>;
  BI it(s.begin());
  BI it2(it);
  REQUIRE_FALSE(it != it2);
  ++it2;
  REQUIRE(it != it2);
}

TEST_CASE(
    "Can copy assign a IteratorWrapper with SameTypes", "[base_iterator]") {
  SameTypes s;
  using BI = IteratorWrapper<SameTypes>;
  BI it(s.begin());
  BI it2(s.begin());
  REQUIRE_FALSE(it != it2);
  ++it2;
  REQUIRE(it != it2);
  it = it2;
  REQUIRE_FALSE(it != it2);
}

TEST_CASE("Can copy construct a IteratorWrapper with DifferentTypes",
    "[base_iterator]") {
  using BI = IteratorWrapper<DifferentTypes>;
  DifferentTypes d;
  BI it(d.begin());
  BI it2(it);
  REQUIRE_FALSE(it != it2);
  ++it;
  REQUIRE(it != it2);
}

TEST_CASE("Can copy construct a IteratorWrapper with DifferenTypes",
    "[base_iterator]") {
  using BI = IteratorWrapper<DifferentTypes>;
  DifferentTypes d;
  BI it(d.begin());
  BI it2(it);
  it = it2;
  // break assignment into a different test
  REQUIRE_FALSE(it != it2);
  BI it_end(d.end());
  REQUIRE(it != it_end);
  SECTION("normal = end") {
    it = it_end;
    REQUIRE_FALSE(it != it_end);
  }
  SECTION("end = normal") {
    it_end = BI{d.begin()};
    REQUIRE_FALSE(it != it_end);
  }
}

// TODO test move operations
