#include <cycle.hpp>

#include "helpers.hpp"

#include <iterator>
#include <string>
#include <vector>

#include "catch.hpp"

using iter::cycle;

TEST_CASE("cycle: iterate twice", "[cycle]") {
  std::vector<int> ns{2, 4, 6};
  std::vector<int> v{};
  std::size_t count = 0;
  for (auto i : cycle(ns)) {
    v.push_back(i);
    ++count;
    if (count == ns.size() * 2) {
      break;
    }
  }

  auto vc = ns;
  vc.insert(std::end(vc), std::begin(ns), std::end(ns));
  REQUIRE(v == vc);
}

TEST_CASE("cycle: Works with different begin and end types", "[cycle]") {
  constexpr auto sz = 'd' - 'a';
  CharRange cr{'d'};
  const std::vector<char> vc{'a', 'b', 'c', 'a', 'b', 'c'};
  std::vector<char> v;
  std::size_t count = 0;
  for (auto i : cycle(cr)) {
    v.push_back(i);
    ++count;
    if (count == sz * 2) {
      break;
    }
  }

  REQUIRE(v == vc);
}

TEST_CASE("cycle: with pipe", "[cycle]") {
  std::vector<int> ns{2, 4, 6};
  std::vector<int> v;
  std::size_t count = 0;
  for (auto i : ns | cycle) {
    v.push_back(i);
    ++count;
    if (count == ns.size() * 2) {
      break;
    }
  }

  auto vc = ns;
  vc.insert(std::end(vc), std::begin(ns), std::end(ns));
  REQUIRE(v == vc);
}

TEST_CASE("cycle: empty cycle terminates", "[cycle]") {
  std::vector<int> ns;
  auto c = cycle(ns);
  std::vector<int> v(std::begin(c), std::end(c));
  REQUIRE(v.empty());
}

TEST_CASE("cycle: binds to lvalues, moves rvalues", "[cycle]") {
  itertest::BasicIterable<char> bi{'x', 'y', 'z'};
  SECTION("binds to lvalues") {
    cycle(bi);
    REQUIRE_FALSE(bi.was_moved_from());
  }
  SECTION("moves rvalues") {
    cycle(std::move(bi));
    REQUIRE(bi.was_moved_from());
  }
}

TEST_CASE("cycle: doesn't move or copy elements of iterable", "[cycle]") {
  constexpr itertest::SolidInt arr[] = {{6}, {7}, {8}};
  auto c = cycle(arr);
  *std::begin(c);
}

TEST_CASE("cycle: iterator meets requirements", "[cycle]") {
  std::string s{};
  auto c = cycle(s);
  REQUIRE(itertest::IsIterator<decltype(std::begin(c))>::value);
}

TEST_CASE("cycle: arrow works", "[cycle]") {
  std::vector<std::string> v = {"hello"};
  auto c = cycle(v);
  auto it = std::begin(c);
  REQUIRE(it->size() == 5);
}

template <typename T>
using ImpT = decltype(cycle(std::declval<T>()));
TEST_CASE("cycle: has correct ctor and assign ops", "[cycle]") {
  REQUIRE(itertest::IsMoveConstructibleOnly<ImpT<std::string&>>::value);
  REQUIRE(itertest::IsMoveConstructibleOnly<ImpT<std::string>>::value);
}
