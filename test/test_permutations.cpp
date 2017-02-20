#include <permutations.hpp>

#include "helpers.hpp"

#include <iterator>
#include <string>
#include <vector>

#include "catch.hpp"

using iter::permutations;
using IntPermSet = std::multiset<std::vector<int>>;

TEST_CASE("permutations: basic test, 3 element sequence", "[permutations]") {
  const std::vector<int> ns = {1, 7, 9};

  IntPermSet v;
  SECTION("Normal call") {
    for (auto&& st : permutations(ns)) {
      v.emplace(std::begin(st), std::end(st));
    }
  }
  SECTION("Pipe") {
    for (auto&& st : ns | permutations) {
      v.emplace(std::begin(st), std::end(st));
    }
  }

  const IntPermSet vc = {
      {1, 7, 9}, {1, 9, 7}, {7, 1, 9}, {7, 9, 1}, {9, 1, 7}, {9, 7, 1}};
  REQUIRE(v == vc);
}

TEST_CASE("permutations: Works with different begin and end types",
    "[permutations]") {
  CharRange cr{'d'};
  using CharPermSet = std::multiset<std::vector<char>>;
  CharPermSet sc;
  for (auto&& v : permutations(cr)) {
    sc.emplace(std::begin(v), std::end(v));
  }
  const CharPermSet ans = {{'a', 'b', 'c'}, {'a', 'c', 'b'}, {'b', 'a', 'c'},
      {'b', 'c', 'a'}, {'c', 'a', 'b'}, {'c', 'b', 'a'}};

  REQUIRE(ans == sc);
}

TEST_CASE(
    "permutations: empty sequence has one empy permutation", "[permutations]") {
  const std::vector<int> ns{};
  auto p = permutations(ns);
  auto it = std::begin(p);
  REQUIRE((*it).empty());
  it++;
  REQUIRE(it == std::end(p));
}

TEST_CASE("permutations: iterators can be compared", "[permutations]") {
  const std::vector<int> ns = {1, 2};
  auto p = permutations(ns);
  auto it = std::begin(p);
  REQUIRE(it == std::begin(p));
  REQUIRE_FALSE(it != std::begin(p));
  REQUIRE(it != std::end(p));
  REQUIRE_FALSE(it == std::end(p));
  ++it;
  REQUIRE_FALSE(it == std::begin(p));
  REQUIRE(it != std::begin(p));
  REQUIRE_FALSE(it == std::end(p));
  REQUIRE(it != std::end(p));
  ++it;
  REQUIRE(it == std::end(p));
  REQUIRE_FALSE(it != std::end(p));
}

TEST_CASE("permutations: binds to lvalues, moves rvalues", "[permutations]") {
  itertest::BasicIterable<int> bi{1, 2};
  SECTION("binds to lvalues") {
    permutations(bi);
    REQUIRE_FALSE(bi.was_moved_from());
  }

  SECTION("moves rvalues") {
    permutations(std::move(bi));
    REQUIRE(bi.was_moved_from());
  }
}

namespace itertest {
  bool operator<(const SolidInt& lhs, const SolidInt& rhs) {
    return lhs.getint() < rhs.getint();
  }
}

TEST_CASE("permutations doesn't move or copy elements of iterable",
    "[permutations]") {
  constexpr itertest::SolidInt arr[] = {{1}, {0}, {2}};
  for (auto&& st : permutations(arr)) {
    (void)st;
  }
}

TEST_CASE("permutations: iterator meets requirements", "[permutations]") {
  std::string s{"abc"};
  auto c = permutations(s);
  REQUIRE(itertest::IsIterator<decltype(std::begin(c))>::value);
  auto&& row = *std::begin(c);
  REQUIRE(itertest::IsIterator<decltype(std::begin(row))>::value);
}

template <typename T>
using ImpT = decltype(permutations(std::declval<T>()));
TEST_CASE("permutations: has correct ctor and assign ops", "[permutations]") {
  REQUIRE(itertest::IsMoveConstructibleOnly<ImpT<std::string&>>::value);
  REQUIRE(itertest::IsMoveConstructibleOnly<ImpT<std::string>>::value);
}
