#include <dropwhile.hpp>

#include "helpers.hpp"

#include <iterator>
#include <string>
#include <vector>

#include "catch.hpp"

using iter::dropwhile;

using Vec = const std::vector<int>;

TEST_CASE("dropwhile: skips initial elements", "[dropwhile]") {
  Vec ns{1, 2, 3, 4, 5, 6, 7, 8};
  std::vector<int> v;
  SECTION("Normal call") {
    auto d = dropwhile([](int i) { return i < 5; }, ns);
    v.assign(std::begin(d), std::end(d));
  }
  SECTION("Pipe") {
    auto d = ns | dropwhile([](int i) { return i < 5; });
    v.assign(std::begin(d), std::end(d));
  }
  Vec vc = {5, 6, 7, 8};
  REQUIRE(v == vc);
}

TEST_CASE(
    "dropwhile: Works with different begin and end types", "[dropwhile]") {
  CharRange cr{'f'};
  auto d = dropwhile([](char c) { return c < 'c'; }, cr);
  Vec v(d.begin(), d.end());
  Vec vc{'c', 'd', 'e'};
  REQUIRE(v == vc);
}

TEST_CASE("dropwhile: doesn't skip anything if it shouldn't", "[dropwhile]") {
  Vec ns{3, 4, 5, 6};
  auto d = dropwhile([](int i) { return i < 3; }, ns);
  Vec v(std::begin(d), std::end(d));
  Vec vc = {3, 4, 5, 6};
  REQUIRE(v == vc);
}

TEST_CASE("dropwhile: skips all elements when all are true under predicate",
    "[dropwhile]") {
  Vec ns{3, 4, 5, 6};
  auto d = dropwhile([](int i) { return i != 0; }, ns);
  REQUIRE(std::begin(d) == std::end(d));
}

TEST_CASE("dropwhile: identity", "[dropwhile]") {
  Vec ns{1, 2, 0, 3, 1, 0};
  auto d = dropwhile(ns);
  Vec v(std::begin(d), std::end(d));
  Vec vc = {0, 3, 1, 0};
  REQUIRE(v == vc);
}

TEST_CASE("dropwhile: empty case is empty", "[dropwhile]") {
  Vec ns{};
  auto d = dropwhile([](int i) { return i != 0; }, ns);
  REQUIRE(std::begin(d) == std::end(d));
}

TEST_CASE("dropwhile: only drops from beginning", "[dropwhile]") {
  Vec ns{1, 2, 3, 4, 5, 6, 5, 4, 3, 2, 1};
  auto d = dropwhile([](int i) { return i < 5; }, ns);
  Vec v(std::begin(d), std::end(d));
  Vec vc = {5, 6, 5, 4, 3, 2, 1};
  REQUIRE(v == vc);
}

TEST_CASE("dropwhile: operator->", "[dropwhile]") {
  std::vector<std::string> vs = {"a", "ab", "abcdef", "abcdefghi"};
  auto d = dropwhile([](const std::string& str) { return str.size() < 3; }, vs);
  auto it = std::begin(d);
  REQUIRE(it->size() == 6);
}

namespace {
  int less_than_five(int i) {
    return i < 5;
  }
}

TEST_CASE("dropwhile: works with function pointer", "[dropwhile]") {
  Vec ns{1, 2, 3, 4, 5, 6, 7, 8};
  auto d = dropwhile(less_than_five, ns);
  Vec v(std::begin(d), std::end(d));
  Vec vc = {5, 6, 7, 8};
  REQUIRE(v == vc);
}

TEST_CASE("dropwhile: binds to lvalues, moves rvalues", "[dropwhile]") {
  itertest::BasicIterable<int> bi{1, 2, 3, 4};
  SECTION("binds to lvalues") {
    dropwhile(less_than_five, bi);
    REQUIRE_FALSE(bi.was_moved_from());
  }
  SECTION("moves rvalues") {
    dropwhile(less_than_five, std::move(bi));
    REQUIRE(bi.was_moved_from());
  }
}

TEST_CASE(
    "dropwhile: doesn't move or copy elements of iterable", "[dropwhile]") {
  constexpr itertest::SolidInt arr[] = {{6}, {7}, {8}};
  for (auto&& i :
      dropwhile([](const itertest::SolidInt&) { return false; }, arr)) {
    (void)i;
  }
}

TEST_CASE("dropwhile: iterator meets requirements", "[dropwhile]") {
  std::string s{};
  auto c = dropwhile([] { return true; }, s);
  REQUIRE(itertest::IsIterator<decltype(std::begin(c))>::value);
}

template <typename T, typename U>
using ImpT = decltype(dropwhile(std::declval<T>(), std::declval<U>()));
TEST_CASE("dropwhile: has correct ctor and assign ops", "[dropwhile]") {
  using T1 = ImpT<bool (*)(char c), std::string&>;
  auto lam = [](char) { return false; };
  using T2 = ImpT<decltype(lam), std::string>;
  REQUIRE(itertest::IsMoveConstructibleOnly<T1>::value);
  REQUIRE(itertest::IsMoveConstructibleOnly<T2>::value);
}
