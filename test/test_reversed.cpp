#include <reversed.hpp>

#include <array>
#include <string>
#include <utility>
#include <vector>

#include "catch.hpp"

#define DECLARE_REVERSE_ITERATOR
#include "helpers.hpp"
#undef DECLARE_REVERSE_ITERATOR

using iter::reversed;
using Vec = const std::vector<int>;

TEST_CASE("reversed: can reverse a vector", "[reversed]") {
  Vec ns = {10, 20, 30, 40};
  std::vector<int> v;
  SECTION("Normal call") {
    auto r = reversed(ns);
    v.assign(std::begin(r), std::end(r));
  }
  SECTION("Pipe") {
    auto r = ns | reversed;
    v.assign(std::begin(r), std::end(r));
  }

  Vec vc = {40, 30, 20, 10};

  REQUIRE(v == vc);
}

#if 0
TEST_CASE("reversed: Works with different begin and end types",
    "[reversed]") {
  CharRange cr{'d'};
  auto r = reversed(cr);
  Vec v(r.begin(), r.end());
  Vec vc{'c', 'b', 'a'};
  REQUIRE(v == vc);
}
#endif

TEST_CASE("reversed: can reverse an array", "[reversed]") {
  int ns[] = {10, 20, 30, 40};
  auto r = reversed(ns);

  Vec v(std::begin(r), std::end(r));
  Vec vc = {40, 30, 20, 10};

  REQUIRE(v == vc);
}

TEST_CASE("reversed: empty when iterable is empty", "[reversed]") {
  Vec emp{};
  auto r = reversed(emp);
  REQUIRE(std::begin(r) == std::end(r));
}

TEST_CASE("reversed: moves rvalues and binds to lvalues", "[reversed]") {
  itertest::BasicIterable<int> bi{1, 2};
  itertest::BasicIterable<int> bi2{1, 2};
  reversed(bi);
  REQUIRE_FALSE(bi.was_moved_from());

  reversed(std::move(bi2));
  REQUIRE(bi2.was_moved_from());
}

TEST_CASE("reversed: doesn't move or copy elements of array", "[reversed]") {
  constexpr itertest::SolidInt arr[] = {{6}, {7}, {8}};
  for (auto&& i : reversed(arr)) {
    (void)i;
  }
}

TEST_CASE("reversed: with iterable doesn't move or copy elems", "[reversed]") {
  constexpr std::array<itertest::SolidInt, 3> arr{{{6}, {7}, {8}}};
  for (auto&& i : reversed(arr)) {
    (void)i;
  }
}

TEST_CASE("reversed: iterator meets requirements", "[reversed]") {
  Vec v;
  auto r = reversed(v);
  REQUIRE(itertest::IsIterator<decltype(std::begin(r))>::value);

  int a[1];
  auto ra = reversed(a);
  REQUIRE(itertest::IsIterator<decltype(std::begin(ra))>::value);
}

template <typename T>
using ImpT = decltype(reversed(std::declval<T>()));
TEST_CASE("reversed: has correct ctor and assign ops", "[reversed]") {
  REQUIRE(itertest::IsMoveConstructibleOnly<ImpT<std::string&>>::value);
  REQUIRE(itertest::IsMoveConstructibleOnly<ImpT<std::string>>::value);
}
