#include <sliding_window.hpp>

#include <array>
#include <string>
#include <utility>
#include <vector>

#include "catch.hpp"
#include "helpers.hpp"

using iter::sliding_window;
using Vec = const std::vector<int>;

TEST_CASE("sliding_window: window of size 3", "[sliding_window]") {
  Vec ns = {10, 20, 30, 40, 50};
  std::vector<std::vector<int>> vc = {{10, 20, 30}, {20, 30, 40}, {30, 40, 50}};
  std::vector<std::vector<int>> v;
  SECTION("Normal call") {
    for (auto&& win : sliding_window(ns, 3)) {
      v.emplace_back(std::begin(win), std::end(win));
    }
  }
  SECTION("Pipe") {
    for (auto&& win : ns | sliding_window(3)) {
      v.emplace_back(std::begin(win), std::end(win));
    }
  }
  REQUIRE(v == vc);
}

TEST_CASE("sliding_window: Works with different begin and end types",
    "[sliding_window]") {
  CharRange cr{'f'};
  std::vector<std::vector<char>> results;
  for (auto&& g : sliding_window(cr, 3)) {
    results.emplace_back(std::begin(g), std::end(g));
  }
  std::vector<std::vector<char>> rc = {
      {'a', 'b', 'c'}, {'b', 'c', 'd'}, {'c', 'd', 'e'}};
  REQUIRE(results == rc);
}

TEST_CASE("sliding window: oversized window is empty", "[sliding_window]") {
  Vec ns = {10, 20, 30};
  auto sw = sliding_window(ns, 5);
  REQUIRE(std::begin(sw) == std::end(sw));
}

TEST_CASE("sliding window: window size == len(iterable)", "[sliding_window]") {
  Vec ns = {10, 20, 30};
  auto sw = sliding_window(ns, 3);
  auto it = std::begin(sw);
  REQUIRE(it != std::end(sw));

  Vec v(std::begin(*it), std::end(*it));

  REQUIRE(ns == v);
  ++it;
  REQUIRE(it == std::end(sw));
}

TEST_CASE("sliding window: empty iterable is empty", "[sliding_window]") {
  Vec ns{};
  auto sw = sliding_window(ns, 1);
  REQUIRE(std::begin(sw) == std::end(sw));
}

TEST_CASE("sliding window: window size of 1", "[sliding_window]") {
  Vec ns = {10, 20, 30};
  auto sw = sliding_window(ns, 1);
  auto it = std::begin(sw);
  REQUIRE(*std::begin(*it) == 10);
  ++it;
  REQUIRE(*std::begin(*it) == 20);
  ++it;
  REQUIRE(*std::begin(*it) == 30);
  ++it;
  REQUIRE(it == std::end(sw));
}

TEST_CASE("sliding window: window size of 0", "[sliding_window]") {
  Vec ns = {10, 20, 30};
  auto sw = sliding_window(ns, 0);
  REQUIRE(std::begin(sw) == std::end(sw));
}

TEST_CASE(
    "sliding window: moves rvalues and binds to lvalues", "[sliding_window]") {
  itertest::BasicIterable<int> bi{1, 2};
  sliding_window(bi, 1);
  REQUIRE_FALSE(bi.was_moved_from());
  sliding_window(std::move(bi), 1);
  REQUIRE(bi.was_moved_from());
}

TEST_CASE("sliding window: doesn't copy elements", "[sliding_window]") {
  constexpr std::array<itertest::SolidInt, 3> arr{{{6}, {7}, {8}}};
  for (auto&& i : sliding_window(arr, 1)) {
    (void)*std::begin(i);
  }
}

TEST_CASE("sliding_window: iterator meets requirements", "[sliding_window]") {
  std::string s{"abcdef"};
  auto c = sliding_window(s, 2);
  REQUIRE(itertest::IsIterator<decltype(std::begin(c))>::value);
}

template <typename T>
using ImpT = decltype(sliding_window(std::declval<T>(), 1));
TEST_CASE(
    "sliding_window: has correct ctor and assign ops", "[sliding_window]") {
  REQUIRE(itertest::IsMoveConstructibleOnly<ImpT<std::string&>>::value);
  REQUIRE(itertest::IsMoveConstructibleOnly<ImpT<std::string>>::value);
}
