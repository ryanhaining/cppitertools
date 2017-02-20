#include <compress.hpp>
#include "helpers.hpp"

#include <iterator>
#include <string>
#include <utility>
#include <vector>

#include "catch.hpp"

using iter::compress;
using itertest::SolidInt;
using itertest::BasicIterable;
using Vec = const std::vector<int>;

TEST_CASE("compress: alternating", "[compress]") {
  std::vector<int> ivec{1, 2, 3, 4, 5, 6};
  std::vector<bool> bvec{true, false, true, false, true, false};
  auto c = compress(ivec, bvec);
  Vec v(std::begin(c), std::end(c));
  Vec vc = {1, 3, 5};

  REQUIRE(v == vc);
}

TEST_CASE("compress: consecutive falses", "[compress]") {
  std::vector<int> ivec{1, 2, 3, 4, 5};
  std::vector<bool> bvec{true, false, false, false, true};
  auto c = compress(ivec, bvec);
  Vec v(std::begin(c), std::end(c));
  Vec vc = {1, 5};

  REQUIRE(v == vc);
}

TEST_CASE("compress: consecutive trues", "[compress]") {
  std::vector<int> ivec{1, 2, 3, 4, 5};
  std::vector<bool> bvec{false, true, true, true, false};
  auto c = compress(ivec, bvec);
  Vec v(std::begin(c), std::end(c));
  Vec vc = {2, 3, 4};

  REQUIRE(v == vc);
}

TEST_CASE("compress: all true", "[compress]") {
  std::vector<int> ivec{1, 2, 3, 4, 5};
  std::vector<bool> bvec(ivec.size(), true);
  auto c = compress(ivec, bvec);
  Vec v(std::begin(c), std::end(c));

  REQUIRE(v == ivec);
}

TEST_CASE("compress: all false", "[compress]") {
  std::vector<int> ivec{1, 2, 3, 4, 5};
  std::vector<bool> bvec(ivec.size(), false);
  auto c = compress(ivec, bvec);
  REQUIRE(std::begin(c) == std::end(c));
}

TEST_CASE("compress: operator->", "[compress") {
  std::vector<std::string> svec = {"a", "abc", "abcde"};
  std::vector<bool> bvec = {false, false, true};
  auto c = compress(svec, bvec);
  auto it = std::begin(c);
  REQUIRE(it->size() == 5);
}

TEST_CASE("compress: binds to lvalues, moves rvalues", "[compress]") {
  BasicIterable<char> bi{'x', 'y', 'z'};
  std::vector<bool> bl{true, true, true};
  SECTION("binds to lvalues") {
    compress(bi, bl);
    REQUIRE_FALSE(bi.was_moved_from());
  }
  SECTION("moves rvalues") {
    compress(std::move(bi), bl);
    REQUIRE(bi.was_moved_from());
  }
}

struct BoolLike {
 public:
  bool state;
  explicit operator bool() const {
    return this->state;
  }
};

TEST_CASE("compress: workds with truthy and falsey values", "[compress]") {
  std::vector<BoolLike> bvec{{true}, {false}, {true}, {false}};

  Vec ivec{1, 2, 3, 4};

  auto c = compress(ivec, bvec);
  Vec v(std::begin(c), std::end(c));

  Vec vc = {1, 3};
  REQUIRE(v == vc);
}

TEST_CASE("compress: terminates on shorter selectors", "[compress]") {
  std::vector<int> ivec{1, 2, 3, 4, 5};
  std::vector<bool> bvec{true};
  auto c = compress(ivec, bvec);
  REQUIRE(std::distance(std::begin(c), std::end(c)) == 1);
}

TEST_CASE("compress: terminates on shorter data", "[compress]") {
  std::vector<int> ivec{1};
  std::vector<bool> bvec{true, true, true, true, true};
  auto c = compress(ivec, bvec);
  REQUIRE(std::distance(std::begin(c), std::end(c)) == 1);
}

TEST_CASE("compress: nothing on empty selectors", "[compress]") {
  std::vector<int> ivec{1, 2, 3};
  std::vector<bool> bvec{};
  auto c = compress(ivec, bvec);
  REQUIRE(std::begin(c) == std::end(c));
}

TEST_CASE("compress: nothing on empty data", "[compress]") {
  std::vector<int> ivec{};
  std::vector<bool> bvec{true, true, true};
  auto c = compress(ivec, bvec);
  REQUIRE(std::begin(c) == std::end(c));
}

TEST_CASE("compress: iterator meets requirements", "[compress]") {
  std::string s{};
  std::vector<bool> bv;
  auto c = compress(s, bv);
  REQUIRE(itertest::IsIterator<decltype(std::begin(c))>::value);
}

TEST_CASE("compress: Works with different begin and end types", "[compress]") {
  CharRange cr{'d'};
  auto c = compress(cr, std::vector<bool>{true, false, true});
  Vec v(c.begin(), c.end());
  Vec vc{'a', 'c'};
  REQUIRE(v == vc);
}

template <typename T, typename U>
using ImpT = decltype(compress(std::declval<T>(), std::declval<U>()));
TEST_CASE("compress: has correct ctor and assign ops", "[compress]") {
  using T1 = ImpT<std::string&, std::vector<bool>&>;
  using T2 = ImpT<std::string, std::vector<bool>>;
  REQUIRE(itertest::IsMoveConstructibleOnly<T1>::value);
  REQUIRE(itertest::IsMoveConstructibleOnly<T2>::value);
}
