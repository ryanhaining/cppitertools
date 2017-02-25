// AGAIN the contents of iterbase are completely subject to change, do not rely
// on any of this.  Users of the library must consider all of this undocumented
//

#include <enumerate.hpp>
#include <internal/iterbase.hpp>
#include <iterator>
#include <list>
#include <string>
#include <type_traits>
#include <vector>

#include "catch.hpp"
#include "helpers.hpp"

namespace it = iter::impl;

using IVec = std::vector<int>;

template <typename T>
using hrai = it::has_random_access_iter<T>;
TEST_CASE("Detects random access iterators correctly", "[iterbase]") {
  REQUIRE(hrai<std::vector<int>>::value);
  REQUIRE(hrai<std::string>::value);
  REQUIRE(hrai<int[10]>::value);

  REQUIRE_FALSE(hrai<std::list<int>>::value);
  REQUIRE_FALSE(hrai<decltype(iter::enumerate(std::list<int>{}))>::value);
  REQUIRE_FALSE(hrai<itertest::BasicIterable<int>>::value);
}

TEST_CASE("Detects correct iterator types", "[iterbase]") {
  REQUIRE((std::is_same<it::iterator_type<IVec>, IVec::iterator>::value));
  REQUIRE((std::is_same<it::iterator_type<IVec>, IVec::iterator>::value));
  REQUIRE((std::is_same<it::iterator_deref<IVec>,
      IVec::iterator::reference>::value));
  REQUIRE((std::is_same<it::const_iterator_deref<IVec>,
      IVec::iterator::reference>::value));
  REQUIRE((std::is_same<it::iterator_traits_deref<IVec>,
      IVec::iterator::value_type>::value));

  REQUIRE(
      (std::is_same<it::iterator_arrow<IVec>, IVec::iterator::pointer>::value));
  REQUIRE((std::is_same<it::iterator_arrow<int[10]>, int*>::value));
}

TEST_CASE("advance, next, size", "[iterbase]") {
  IVec v = {2, 4, 6, 8, 10, 12, 14, 16, 18};
  auto itr = std::begin(v);
  REQUIRE(it::apply_arrow(itr) == &v[0]);

  it::dumb_advance_unsafe(itr, 3);
  REQUIRE(itr == (std::begin(v) + 3));
  REQUIRE(it::dumb_next(std::begin(v), 3) == std::begin(v) + 3);
  REQUIRE(it::dumb_size(v) == v.size());
}

TEST_CASE("are_same", "[iterbase]") {
  REQUIRE((it::are_same<int, int, int, int>::value));
  REQUIRE_FALSE((it::are_same<double, int, int, int>::value));
  REQUIRE_FALSE((it::are_same<int, int, int, double>::value));
  REQUIRE_FALSE((it::are_same<int, double, int, int>::value));
}

TEST_CASE("DerefHolder lvalue reference", "[iterbase]") {
  it::DerefHolder<int&> dh;
  int a = 2;
  int b = 5;
  REQUIRE_FALSE(dh);
  dh.reset(a);
  REQUIRE(dh);

  REQUIRE(dh.get_ptr() == &a);
  REQUIRE(&dh.get() == &a);
  dh.reset(b);
  REQUIRE(dh.get_ptr() == &b);
  REQUIRE(&dh.get() == &b);
}

TEST_CASE("DerefHolder non-reference", "[iterbase]") {
  it::DerefHolder<int> dh;
  int a = 2;
  int b = 5;
  REQUIRE_FALSE(dh);
  dh.reset(std::move(a));
  REQUIRE(dh.get() == 2);
  REQUIRE(&dh.get() != &a);

  dh.reset(std::move(b));
  REQUIRE(dh.get() == 5);
}

TEST_CASE("get_begin returns correct type", "[iterbase]") {
  std::vector<int> v;
  REQUIRE((std::is_same<decltype(it::get_begin(v)), decltype(v.begin())>{}));
}
