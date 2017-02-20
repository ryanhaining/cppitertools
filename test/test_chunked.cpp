#include <chunked.hpp>

#include <array>
#include <string>
#include <utility>
#include <vector>

#include "catch.hpp"
#include "helpers.hpp"

using iter::chunked;
using Vec = std::vector<int>;
using ResVec = std::vector<Vec>;

TEST_CASE("chunked: basic test", "[chunked]") {
  Vec ns = {1, 2, 3, 4, 5, 6};
  ResVec results;
  SECTION("Normal call") {
    for (auto&& g : chunked(ns, 2)) {
      results.emplace_back(std::begin(g), std::end(g));
    }
  }
  SECTION("Pipe") {
    for (auto&& g : ns | chunked(2)) {
      results.emplace_back(std::begin(g), std::end(g));
    }
  }

  ResVec rc = {{1, 2}, {3, 4}, {5, 6}};

  REQUIRE(results == rc);
}

TEST_CASE("chunked: len(iterable) % groupsize != 0", "[chunked]") {
  Vec ns = {1, 2, 3, 4, 5, 6, 7};
  ResVec results;
  for (auto&& g : chunked(ns, 3)) {
    results.emplace_back(std::begin(g), std::end(g));
  }

  ResVec rc = {{1, 2, 3}, {4, 5, 6}, {7}};

  REQUIRE(results == rc);
}

TEST_CASE("chunked: iterators can be compared", "[chunked]") {
  Vec ns = {1, 2, 3, 4, 5, 6, 7};
  auto g = chunked(ns, 3);
  auto it = std::begin(g);
  REQUIRE(it == std::begin(g));
  REQUIRE_FALSE(it != std::begin(g));
  ++it;
  REQUIRE(it != std::begin(g));
  REQUIRE_FALSE(it == std::begin(g));
}

TEST_CASE("chunked: size 0 is empty", "[chunked]") {
  Vec ns{1, 2, 3};
  auto g = chunked(ns, 0);
  REQUIRE(std::begin(g) == std::end(g));
}

TEST_CASE("chunked: Works with different begin and end types", "[chunked]") {
  CharRange cr{'f'};
  std::vector<std::vector<char>> results;
  for (auto&& g : chunked(cr, 3)) {
    results.emplace_back(std::begin(g), std::end(g));
  }
  std::vector<std::vector<char>> rc = {{'a', 'b', 'c'}, {'d', 'e'}};
  REQUIRE(results == rc);
}

TEST_CASE("chunked: empty iterable gives empty chunked", "[chunked]") {
  Vec ns{};
  auto g = chunked(ns, 1);
  REQUIRE(std::begin(g) == std::end(g));
}

TEST_CASE("chunked: iterator meets requirements", "[chunked]") {
  std::string s{};
  auto c = chunked(s, 1);
  REQUIRE(itertest::IsIterator<decltype(std::begin(c))>::value);
}

template <typename T>
using ImpT = decltype(chunked(std::declval<T>(), 1));
TEST_CASE("chunked: has correct ctor and assign ops", "[chunked]") {
  REQUIRE(itertest::IsMoveConstructibleOnly<ImpT<std::string&>>::value);
  REQUIRE(itertest::IsMoveConstructibleOnly<ImpT<std::string>>::value);
}
