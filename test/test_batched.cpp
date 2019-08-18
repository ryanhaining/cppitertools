#include <batched.hpp>

#include <array>
#include <string>
#include <utility>
#include <vector>

#include "catch.hpp"
#include "helpers.hpp"

using iter::batched;
using Vec = std::vector<int>;
using ResVec = std::vector<Vec>;

TEST_CASE("batched: basic test", "[batched]") {
  Vec ns = {1, 2, 3, 4, 5, 6};
  ResVec results;
  SECTION("Normal call") {
    for (auto&& g : batched(ns, 2)) {
      results.emplace_back(std::begin(g), std::end(g));
    }
  }
  SECTION("Pipe") {
    for (auto&& g : ns | batched(2)) {
      results.emplace_back(std::begin(g), std::end(g));
    }
  }

  ResVec rc = {{1, 2, 3}, {4 ,5, 6}};

  REQUIRE(results == rc);
}

TEST_CASE("batched: uneven batch sizes", "[batched]") {
  Vec ns = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  ResVec results;
  for (auto&& g : batched(ns, 6)) {
    results.emplace_back(std::begin(g), std::end(g));
  }

  ResVec rc = {{1, 2}, {3, 4}, {5, 6}, {7}, {8}, {9}};

  REQUIRE(results == rc);
}

TEST_CASE("batched: const batched", "[batched][const]") {
  Vec ns = {1, 2, 3, 4, 5, 6};
  ResVec results;
  SECTION("Normal call") {
    const auto& ch = batched(ns, 2);
    for (auto&& g : ch) {
      results.emplace_back(std::begin(g), std::end(g));
    }
  }
  ResVec rc = {{1, 2, 3}, {4, 5, 6}};

  REQUIRE(results == rc);
}

TEST_CASE("batched: const iterators can be compared to non-const iterators",
    "[batched][const]") {
  auto c = batched(Vec{}, 1);
  const auto& cc = c;
  (void)(std::begin(c) == std::end(cc));
}

TEST_CASE("batched: len(iterable) % num_batches != 0", "[batched]") {
  Vec ns = {1, 2, 3, 4, 5, 6, 7};
  ResVec results;
  for (auto&& g : batched(ns, 3)) {
    results.emplace_back(std::begin(g), std::end(g));
  }

  ResVec rc = {{1, 2, 3}, {4, 5}, {6, 7}};

  REQUIRE(results == rc);
}

TEST_CASE("batched: num_batches > len(iterable)", "[batched]") {
  Vec ns = {1, 2, 3, 4, 5, 6, 7};
  ResVec results;
  for (auto&& g : batched(ns, 9)) {
    results.emplace_back(std::begin(g), std::end(g));
  }

  ResVec rc = {{1}, {2}, {3}, {4}, {5}, {6}, {7}};

  REQUIRE(results == rc);
}

TEST_CASE("batched: iterators can be compared", "[batched]") {
  Vec ns = {1, 2, 3, 4, 5, 6, 7};
  auto g = batched(ns, 3);
  auto it = std::begin(g);
  REQUIRE(it == std::begin(g));
  REQUIRE_FALSE(it != std::begin(g));
  ++it;
  REQUIRE(it != std::begin(g));
  REQUIRE_FALSE(it == std::begin(g));
}

TEST_CASE("batched: size 0 is empty", "[batched]") {
  Vec ns{1, 2, 3};
  auto g = batched(ns, 0);
  REQUIRE(std::begin(g) == std::end(g));
}

TEST_CASE("batched: Works with different begin and end types", "[batched]") {
  CharRange cr{'f'};
  std::vector<std::vector<char>> results;
  for (auto&& g : batched(cr, 3)) {
    results.emplace_back(std::begin(g), std::end(g));
  }
  std::vector<std::vector<char>> rc = {{'a', 'b'}, {'c', 'd'}, {'e'}};
  REQUIRE(results == rc);
}

TEST_CASE("batched: empty iterable gives empty batched", "[batched]") {
  Vec ns{};
  auto g = batched(ns, 1);
  REQUIRE(std::begin(g) == std::end(g));
}

TEST_CASE("batched: iterator meets requirements", "[batched]") {
  std::string s{};
  auto c = batched(s, 1);
  REQUIRE(itertest::IsIterator<decltype(std::begin(c))>::value);
}

template <typename T>
using ImpT = decltype(batched(std::declval<T>(), 1));
TEST_CASE("batched: has correct ctor and assign ops", "[batched]") {
  REQUIRE(itertest::IsMoveConstructibleOnly<ImpT<std::string&>>::value);
  REQUIRE(itertest::IsMoveConstructibleOnly<ImpT<std::string>>::value);
}
