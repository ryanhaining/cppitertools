#include <takewhile.hpp>

#include <array>
#include <string>
#include <utility>
#include <vector>

#include "catch.hpp"
#include "helpers.hpp"

using iter::takewhile;
using Vec = const std::vector<int>;

namespace {
  bool under_ten(int i) {
    return i < 10;
  }

  struct UnderTen {
    bool operator()(int i) {
      return i < 10;
    }
  };
}

TEST_CASE("takewhile: works with lambda, callable, and function pointer",
    "[takewhile]") {
  Vec ns = {1, 3, 5, 20, 2, 4, 6, 8};
  SECTION("function pointer") {
    auto tw = takewhile(under_ten, ns);
    Vec v(std::begin(tw), std::end(tw));
    Vec vc = {1, 3, 5};
    REQUIRE(v == vc);
  }

  SECTION("callable object") {
    std::vector<int> v;
    SECTION("Normal call") {
      auto tw = takewhile(UnderTen{}, ns);
      v.assign(std::begin(tw), std::end(tw));
    }

    SECTION("Pipe") {
      auto tw = ns | takewhile(UnderTen{});
      v.assign(std::begin(tw), std::end(tw));
    }

    Vec vc = {1, 3, 5};
    REQUIRE(v == vc);
  }

  SECTION("lambda") {
    auto tw = takewhile([](int i) { return i < 10; }, ns);
    Vec v(std::begin(tw), std::end(tw));
    Vec vc = {1, 3, 5};
    REQUIRE(v == vc);
  }
}

TEST_CASE(
    "takewhile: Works with different begin and end types", "[takewhile]") {
  CharRange cr{'f'};
  auto t = takewhile([](char c) { return c < 'd'; }, cr);
  Vec v(t.begin(), t.end());
  Vec vc{'a', 'b', 'c'};
  REQUIRE(v == vc);
}

TEST_CASE("takewhile: identity", "[takewhile]") {
  Vec ns{1, 2, 3, 0, 4, 5, 0};
  std::vector<int> v;
  SECTION("Normal call") {
    auto tw = takewhile(ns);
    v.assign(std::begin(tw), std::end(tw));
  }
  SECTION("Pipe") {
    auto tw = ns | takewhile;
    v.assign(std::begin(tw), std::end(tw));
  }
  Vec vc = {1, 2, 3};
  REQUIRE(v == vc);
}

TEST_CASE("takewhile: everything passes predicate", "[takewhile]") {
  Vec ns{1, 2, 3};
  auto tw = takewhile(under_ten, ns);
  Vec v(std::begin(tw), std::end(tw));
  Vec vc = {1, 2, 3};
}

TEST_CASE("takewhile: empty iterable is empty", "[takewhile]") {
  Vec ns{};
  auto tw = takewhile(under_ten, ns);
  REQUIRE(std::begin(tw) == std::end(tw));
}

TEST_CASE(
    "takewhile: when first element fails predicate, it's empty"
    "[takewhile]") {
  SECTION("First element is only element") {
    Vec ns = {20};
    auto tw = takewhile(under_ten, ns);
    REQUIRE(std::begin(tw) == std::end(tw));
  }

  SECTION("First element followed by elements that pass") {
    Vec ns = {20, 1, 1};
    auto tw = takewhile(under_ten, ns);
    REQUIRE(std::begin(tw) == std::end(tw));
  }
}

TEST_CASE("takewhile: moves rvalues, binds to lvalues", "[takewhile]") {
  itertest::BasicIterable<int> bi{1, 2};
  takewhile(under_ten, bi);
  REQUIRE_FALSE(bi.was_moved_from());

  takewhile(under_ten, std::move(bi));
  REQUIRE(bi.was_moved_from());
}

TEST_CASE(
    "takewhile: with iterable doesn't move or copy elements", "[takewhile]") {
  constexpr std::array<itertest::SolidInt, 3> arr{{{8}, {9}, {10}}};
  auto func = [](const itertest::SolidInt& si) { return si.getint() < 10; };
  for (auto&& i : takewhile(func, arr)) {
    (void)i;
  }
}

TEST_CASE("takewhile: iterator meets requirements", "[takewhile]") {
  std::string s{};
  auto c = takewhile([] { return true; }, s);
  REQUIRE(itertest::IsIterator<decltype(std::begin(c))>::value);
}

template <typename T, typename U>
using ImpT = decltype(takewhile(std::declval<T>(), std::declval<U>()));
TEST_CASE("takewhile: has correct ctor and assign ops", "[takewhile]") {
  using T1 = ImpT<bool (*)(char c), std::string&>;
  auto lam = [](char) { return false; };
  using T2 = ImpT<decltype(lam), std::string>;
  REQUIRE(itertest::IsMoveConstructibleOnly<T1>::value);
  REQUIRE(itertest::IsMoveConstructibleOnly<T2>::value);
}
