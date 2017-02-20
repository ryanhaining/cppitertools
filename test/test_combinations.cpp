#define DEFINE_DEFAULT_ITERATOR_CTOR
#define CHAR_RANGE_DEFAULT_CONSTRUCTIBLE
#include "helpers.hpp"
#undef CHAR_RANGE_DEFAULT_CONSTRUCTIBLE
#undef DEFINE_DEFAULT_ITERATOR_CTOR

#include <combinations.hpp>

#include <iterator>
#include <set>
#include <string>
#include <vector>

#include "catch.hpp"

using iter::combinations;
using itertest::BasicIterable;
using itertest::SolidInt;
using CharCombSet = std::vector<std::vector<char>>;

TEST_CASE("combinations: Simple combination of 4", "[combinations]") {
  std::string s{"ABCD"};
  CharCombSet sc;
  SECTION("Normal call") {
    for (auto&& v : combinations(s, 2)) {
      sc.emplace_back(std::begin(v), std::end(v));
    }
  }
  SECTION("Pipe") {
    for (auto&& v : s | combinations(2)) {
      sc.emplace_back(std::begin(v), std::end(v));
    }
  }

  CharCombSet ans = {
      {'A', 'B'}, {'A', 'C'}, {'A', 'D'}, {'B', 'C'}, {'B', 'D'}, {'C', 'D'}};
  REQUIRE(ans == sc);
}

TEST_CASE("combinations: Works with different begin and end types",
    "[combinations]") {
  CharRange cr{'e'};
  CharCombSet sc;
  for (auto&& v : combinations(cr, 2)) {
    sc.emplace_back(std::begin(v), std::end(v));
  }
  CharCombSet ans = {
      {'a', 'b'}, {'a', 'c'}, {'a', 'd'}, {'b', 'c'}, {'b', 'd'}, {'c', 'd'}};
  REQUIRE(ans == sc);
}

TEST_CASE("combinations: iterators can be compared", "[combinations]") {
  std::string s{"ABCD"};
  auto c = combinations(s, 2);
  auto it = std::begin(c);
  REQUIRE(it == std::begin(c));
  REQUIRE_FALSE(it != std::begin(c));
  ++it;
  REQUIRE(it != std::begin(c));
  REQUIRE_FALSE(it == std::begin(c));
}

TEST_CASE("combinations: operator->", "[combinations]") {
  std::string s{"ABCD"};
  auto c = combinations(s, 2);
  auto it = std::begin(c);
  REQUIRE(it->size() == 2);
}

TEST_CASE("combinations: size too large gives no results", "[combinations]") {
  std::string s{"ABCD"};
  auto c = combinations(s, 5);
  REQUIRE(std::begin(c) == std::end(c));
}

TEST_CASE("combinations: size 0 gives nothing", "[combinations]") {
  std::string s{"ABCD"};
  auto c = combinations(s, 0);
  REQUIRE(std::begin(c) == std::end(c));
}

TEST_CASE(
    "combinations: iterable without operator*() const", "[combinations]") {
  BasicIterable<char> bi{'x', 'y', 'z'};
  auto c = combinations(bi, 1);
  auto it = std::begin(c);
  ++it;
  (*it)[0];
}

TEST_CASE("combinations: binds to lvalues, moves rvalues", "[combinations]") {
  BasicIterable<char> bi{'x', 'y', 'z'};
  SECTION("binds to lvalues") {
    combinations(bi, 1);
    REQUIRE_FALSE(bi.was_moved_from());
  }
  SECTION("moves rvalues") {
    combinations(std::move(bi), 1);
    REQUIRE(bi.was_moved_from());
  }
}

TEST_CASE("combinations: doesn't move or copy elements of iterable",
    "[combinations]") {
  constexpr SolidInt arr[] = {{6}, {7}, {8}};
  for (auto&& i : combinations(arr, 1)) {
    (void)i;
  }
}

TEST_CASE("combinations: iterator meets requirements", "[combinations]") {
  std::string s{"abc"};
  auto c = combinations(s, 1);
  REQUIRE(itertest::IsIterator<decltype(std::begin(c))>::value);
  auto&& row = *std::begin(c);
  REQUIRE(itertest::IsIterator<decltype(std::begin(row))>::value);
}

template <typename T>
using ImpT = decltype(combinations(std::declval<T>(), 1));
TEST_CASE("combinations: has correct ctor and assign ops", "[combinations]") {
  REQUIRE(itertest::IsMoveConstructibleOnly<ImpT<std::string&>>::value);
  REQUIRE(itertest::IsMoveConstructibleOnly<ImpT<std::string>>::value);
}
