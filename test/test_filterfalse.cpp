#include <cppitertools/filterfalse.hpp>
#include <iterator>
#include <string>
#include <vector>

#include "catch.hpp"
#include "helpers.hpp"

using iter::filterfalse;

using Vec = const std::vector<int>;

TEST_CASE("filterfalse: handles different callable types", "[filterfalse]") {
  Vec ns = {1, 2, 5, 6, 3, 1, 7, -1, 5};
  Vec vc = {5, 6, 7, 5};
  std::vector<int> v;
  SECTION("with function pointer") {
    auto f = filterfalse(less_than_five, ns);
    v = Vec(std::begin(f), std::end(f));
  }

  SECTION("with callable object") {
    auto f = filterfalse(LessThanValue{5}, ns);
    v = Vec(std::begin(f), std::end(f));
  }

  SECTION("with lvalue callable object") {
    auto lt = LessThanValue{5};
    SECTION("normal call") {
      auto f = filterfalse(lt, ns);
      v = Vec(std::begin(f), std::end(f));
    }
    SECTION("pipe") {
      auto f = ns | filterfalse(lt);
      v = Vec(std::begin(f), std::end(f));
    }
  }

  SECTION("with move-only callable object") {
    SECTION("normal call") {
      auto f = filterfalse(MoveOnlyLessThanValue{5}, ns);
      v = Vec(std::begin(f), std::end(f));
    }
    SECTION("pipe") {
      auto f = ns | filterfalse(MoveOnlyLessThanValue{5});
      v = Vec(std::begin(f), std::end(f));
    }
  }

  SECTION("with lambda") {
    auto ltf = [](int i) { return i < 5; };
    auto f = filterfalse(ltf, ns);
    v = Vec(std::begin(f), std::end(f));
  }
  REQUIRE(v == vc);
}

TEST_CASE("filterfalse: handles pointer to member", "[filterfalse]") {
  using itertest::Point;
  const std::vector<Point> ps = {{0, 3}, {4, 0}, {0, 1}, {-1, -1}};
  std::vector<Point> v;
  SECTION("with pointer to data member") {
    auto f = filterfalse(&Point::x, ps);
    v.assign(std::begin(f), std::end(f));
  }

  SECTION("with pointer to member function") {
    auto f = filterfalse(&Point::get_x, ps);
    v.assign(std::begin(f), std::end(f));
  }

  const std::vector<Point> vc = {{0, 3}, {0, 1}};
  REQUIRE(v == vc);
}

TEST_CASE("filterfalse: const iteration", "[filterfalse][const]") {
  Vec ns = {1, 2, 5, 6, 3, 1, 7, -1, 5};
  const auto f = filterfalse(LessThanValue{5}, ns);
  Vec v(std::begin(f), std::end(f));
  Vec vc = {5, 6, 7, 5};
  REQUIRE(v == vc);
}

TEST_CASE("filterfalse: const iterator and non-const iterator can be compared",
    "[filterfalse][const]") {
  auto f = filterfalse(LessThanValue{5}, Vec{});
  const auto& cf = f;
  (void)(std::begin(f) == std::end(cf));
}

TEST_CASE(
    "filterfalse: Works with different begin and end types", "[filterfalse]") {
  CharRange cr{'d'};
  auto f = filterfalse([](char c) { return c == 'b'; }, cr);
  Vec v(f.begin(), f.end());
  Vec vc{'a', 'c'};
  REQUIRE(v == vc);
}

TEST_CASE("filterfalse: using identity", "[filterfalse]") {
  Vec ns{0, 1, 2, 0, 3, 0, 0, 0, 4, 5, 0};
  std::vector<int> v;
  SECTION("Normal call") {
    auto f = filterfalse(ns);
    v.assign(std::begin(f), std::end(f));
  }

  SECTION("Pipe") {
    auto f = ns | filterfalse;
    v.assign(std::begin(f), std::end(f));
  }

  Vec vc = {0, 0, 0, 0, 0, 0};
  REQUIRE(v == vc);
}

TEST_CASE("filterfalse: binds to lvalues, moves rvales", "[filterfalse]") {
  itertest::BasicIterable<int> bi{1, 2, 3, 4};

  SECTION("one-arg binds to lvalues") {
    filterfalse(bi);
    REQUIRE_FALSE(bi.was_moved_from());
  }

  SECTION("two-arg binds to lvalues") {
    filterfalse(less_than_five, bi);
    REQUIRE_FALSE(bi.was_moved_from());
  }

  SECTION("one-arg moves rvalues") {
    filterfalse(std::move(bi));
    REQUIRE(bi.was_moved_from());
  }

  SECTION("two-arg moves rvalues") {
    filterfalse(less_than_five, std::move(bi));
    REQUIRE(bi.was_moved_from());
  }
}

TEST_CASE("filterfalse: all elements pass predicate", "[filterfalse]") {
  Vec ns{0, 1, 2, 3, 4};
  auto f = filterfalse(less_than_five, ns);

  REQUIRE(std::begin(f) == std::end(f));
}

TEST_CASE("filterfalse: iterator meets requirements", "[filterfalse]") {
  std::string s{};
  auto c = filterfalse([] { return true; }, s);
  REQUIRE(itertest::IsIterator<decltype(std::begin(c))>::value);
}

template <typename T, typename U>
using ImpT = decltype(filterfalse(std::declval<T>(), std::declval<U>()));
TEST_CASE("filterfalse: has correct ctor and assign ops", "[filterfalse]") {
  using T1 = ImpT<bool (*)(char c), std::string&>;
  auto lam = [](char) { return false; };
  using T2 = ImpT<decltype(lam), std::string>;
  REQUIRE(itertest::IsMoveConstructibleOnly<T1>::value);
  REQUIRE(itertest::IsMoveConstructibleOnly<T2>::value);
}
