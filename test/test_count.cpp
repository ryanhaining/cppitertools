#include <count.hpp>
#include "helpers.hpp"

#include <iterator>
#include <utility>
#include <vector>

#include "catch.hpp"

using iter::count;

TEST_CASE("count: watch for 10 elements", "[count]") {
  std::vector<int> v{};
  for (auto i : count()) {
    v.push_back(i);
    if (i == 9) break;
  }

  const std::vector<int> vc{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  REQUIRE(v == vc);
}

TEST_CASE("count: start at 10", "[count]") {
  std::vector<int> v{};
  for (auto i : count(10)) {
    v.push_back(i);
    if (i == 14) break;
  }

  const std::vector<int> vc{10, 11, 12, 13, 14};
  REQUIRE(v == vc);
}

TEST_CASE("count: with step", "[count]") {
  std::vector<int> v{};
  for (auto i : count(2, -1)) {
    v.push_back(i);
    if (i == -3) break;
  }

  const std::vector<int> vc{2, 1, 0, -1, -2, -3};
  REQUIRE(v == vc);
}

TEST_CASE("count: with step > 1", "[count]") {
  std::vector<int> v{};
  for (auto i : count(10, 2)) {
    v.push_back(i);
    if (i == 16) break;
  }

  const std::vector<int> vc{10, 12, 14, 16};
  REQUIRE(v == vc);
}

TEST_CASE("count: can bo constexpr", "[count]") {
  constexpr auto c = count();
  constexpr auto c2 = count(5);
  (void)c2;
  constexpr auto c3 = count(5, 2);
  (void)c3;

  constexpr auto it = c.begin();
  constexpr auto i = *it;
  static_assert(i == 0, "count begin not correct value");
}

TEST_CASE("count: iterator meets requirements", "[count]") {
  auto c = count();
  REQUIRE(itertest::IsIterator<decltype(std::begin(c))>::value);
}
