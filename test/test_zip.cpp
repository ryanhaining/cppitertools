#include <zip.hpp>

#include "helpers.hpp"

#include <iterator>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "catch.hpp"

using iter::zip;
using itertest::BasicIterable;
using itertest::SolidInt;

TEST_CASE("zip: Simple case, same length", "[zip]") {
  using Tu = std::tuple<int, char, double>;
  using ResVec = const std::vector<Tu>;
  std::vector<int> iv{10, 20, 30};
  std::string s{"hey"};
  double arr[] = {1.0, 2.0, 4.0};

  auto z = zip(iv, s, arr);
  ResVec v(std::begin(z), std::end(z));
  ResVec vc{Tu{10, 'h', 1.0}, Tu{20, 'e', 2.0}, Tu{30, 'y', 4.0}};
  REQUIRE(v == vc);
}

TEST_CASE(
    "zip: three sequences, one sequence has different begin and end", "[zip]") {
  using Tu = std::tuple<int, char, double>;
  using ResVec = const std::vector<Tu>;
  std::vector<int> iv{10, 20, 30};
  CharRange cr('d');
  double arr[] = {1.0, 2.0, 4.0};

  auto z = zip(iv, cr, arr);
  ResVec v(std::begin(z), std::end(z));
  ResVec vc{Tu{10, 'a', 1.0}, Tu{20, 'b', 2.0}, Tu{30, 'c', 4.0}};
  REQUIRE(v == vc);
}

TEST_CASE("zip: One empty, all empty", "[zip]") {
  std::vector<int> iv = {1, 2, 3};
  std::string s{};
  auto z = zip(iv, s);
  REQUIRE_FALSE(std::begin(z) != std::end(z));
  auto z2 = zip(s, iv);
  REQUIRE_FALSE(std::begin(z2) != std::end(z2));
}

TEST_CASE("zip: terminates on shortest sequence", "[zip]") {
  std::vector<int> iv{1, 2, 3, 4, 5};
  std::string s{"hi"};
  auto z = zip(iv, s);

  REQUIRE(std::distance(std::begin(z), std::end(z)) == 2);
}

TEST_CASE("zip: Empty", "[zip]") {
  auto z = zip();
  REQUIRE_FALSE(std::begin(z) != std::end(z));
}

TEST_CASE("zip: Modify sequence through zip", "[zip]") {
  std::vector<int> iv{1, 2, 3};
  std::vector<int> iv2{1, 2, 3, 4};
  for (auto&& t : zip(iv, iv2)) {
    std::get<0>(t) = -1;
    std::get<1>(t) = -1;
  }

  const std::vector<int> vc{-1, -1, -1};
  const std::vector<int> vc2{-1, -1, -1, 4};
  REQUIRE(iv == vc);
  REQUIRE(iv2 == vc2);
}

TEST_CASE("zip: binds reference when it should", "[zip]") {
  BasicIterable<char> bi{'x', 'y', 'z'};
  zip(bi);
  REQUIRE_FALSE(bi.was_moved_from());
}

TEST_CASE("zip: moves rvalues", "[zip]") {
  BasicIterable<char> bi{'x', 'y', 'z'};
  zip(std::move(bi));
  REQUIRE(bi.was_moved_from());
}

TEST_CASE("zip: Can bind ref and move in single zip", "[zip]") {
  BasicIterable<char> b1{'x', 'y', 'z'};
  BasicIterable<char> b2{'a', 'b'};
  zip(b1, std::move(b2));
  REQUIRE_FALSE(b1.was_moved_from());
  REQUIRE(b2.was_moved_from());
}

TEST_CASE("zip: doesn't move or copy elements of iterable", "[zip]") {
  constexpr SolidInt arr[] = {{6}, {7}, {8}};
  for (auto&& t : zip(arr)) {
    (void)std::get<0>(t);
  }
}

TEST_CASE("zip: postfix ++", "[zip]") {
  const std::vector<int> v = {1};
  auto z = zip(v);
  auto it = std::begin(z);
  it++;
  REQUIRE(it == std::end(z));
}

TEST_CASE("zip: iterator meets requirements", "[zip]") {
  std::string s{};
  auto c = zip(s);
  REQUIRE(itertest::IsIterator<decltype(std::begin(c))>::value);
  auto c2 = zip(s, s);
  REQUIRE(itertest::IsIterator<decltype(std::begin(c2))>::value);
}

template <typename... Ts>
using ImpT = decltype(zip(std::declval<Ts>()...));
TEST_CASE("zip: has correct ctor and assign ops", "[zip]") {
  using T = ImpT<std::string&, std::vector<double>, std::vector<std::string>>;
  REQUIRE(itertest::IsMoveConstructibleOnly<T>::value);
}
