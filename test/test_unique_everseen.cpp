#include <unique_everseen.hpp>

#include "helpers.hpp"

#include <iterator>
#include <string>
#include <vector>

#include "catch.hpp"

using iter::unique_everseen;

using Vec = const std::vector<int>;

TEST_CASE("unique everseen: adjacent repeating values", "[unique_everseen]") {
  Vec ns = {1, 1, 1, 2, 2, 3, 4, 4, 5, 6, 7, 8, 8, 8, 8, 9, 9};
  auto ue = unique_everseen(ns);
  Vec v(std::begin(ue), std::end(ue));
  Vec vc = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  REQUIRE(v == vc);
}

TEST_CASE(
    "unique everseen: nonadjacent repeating values", "[unique_everseen]") {
  Vec ns = {1, 2, 3, 4, 3, 2, 1, 5, 6};
  std::vector<int> v;
  SECTION("Normal call") {
    auto ue = unique_everseen(ns);
    v.assign(std::begin(ue), std::end(ue));
  }
  SECTION("Pipe") {
    auto ue = ns | unique_everseen;
    v.assign(std::begin(ue), std::end(ue));
  }
  Vec vc = {1, 2, 3, 4, 5, 6};
  REQUIRE(v == vc);
}

TEST_CASE(
    "unique everseen: moves rvalues, binds to lvalues", "[unique_everseen]") {
  itertest::BasicIterable<int> bi{1, 2};
  unique_everseen(bi);
  REQUIRE_FALSE(bi.was_moved_from());

  unique_everseen(std::move(bi));
  REQUIRE(bi.was_moved_from());
}

TEST_CASE("unique everseen: Works with different begin and end types",
    "[unique_everseen]") {
  CharRange cr{'d'};
  using Vec = std::vector<char>;
  auto ue = unique_everseen(cr);
  Vec v(ue.begin(), ue.end());
  Vec vc{'a', 'b', 'c'};
  REQUIRE(v == vc);
}

TEST_CASE("unique_everseen: iterator meets requirements", "[unique_everseen]") {
  std::string s{};
  auto c = unique_everseen(s);
  REQUIRE(itertest::IsIterator<decltype(std::begin(c))>::value);
}

template <typename T>
using ImpT = decltype(unique_everseen(std::declval<T>()));
TEST_CASE(
    "unique_everseen: has correct ctor and assign ops", "[unique_everseen]") {
  REQUIRE(itertest::IsMoveConstructibleOnly<ImpT<std::string&>>::value);
  REQUIRE(itertest::IsMoveConstructibleOnly<ImpT<std::string>>::value);
}
