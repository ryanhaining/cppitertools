#include <slice.hpp>

#include <string>
#include <utility>
#include <vector>

#include "catch.hpp"
#include "helpers.hpp"

using iter::slice;
using Vec = const std::vector<int>;

TEST_CASE("slice: take from beginning", "[slice]") {
  Vec ns = {10, 11, 12, 13, 14, 15, 16, 17, 18, 19};

  std::vector<int> v;
  SECTION("Normal call") {
    auto sl = slice(ns, 5);
    v.assign(std::begin(sl), std::end(sl));
  }
  SECTION("Pipe") {
    auto sl = ns | slice(5);
    v.assign(std::begin(sl), std::end(sl));
  }

  Vec vc = {10, 11, 12, 13, 14};
  REQUIRE(v == vc);
}

TEST_CASE("slice: start and stop", "[slice]") {
  Vec ns = {10, 11, 12, 13, 14, 15, 16, 17, 18, 19};

  std::vector<int> v;
  SECTION("Normal call") {
    auto sl = slice(ns, 2, 6);
    v.assign(std::begin(sl), std::end(sl));
  }
  SECTION("Pipe") {
    auto sl = ns | slice(2, 6);
    v.assign(std::begin(sl), std::end(sl));
  }

  Vec vc = {12, 13, 14, 15};
  REQUIRE(v == vc);
}

TEST_CASE("slice: Works with different begin and end types", "[slice]") {
  CharRange cr{'z'};
  auto sl = slice(cr, 2, 5);
  std::vector<char> v(std::begin(sl), std::end(sl));
  const std::vector<char> vc = {'c', 'd', 'e'};
  REQUIRE(v == vc);
}

TEST_CASE("slice: start, stop, step", "[slice]") {
  Vec ns = {10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
  std::vector<int> v;

  SECTION("Normal call") {
    auto sl = slice(ns, 2, 8, 2);
    v.assign(std::begin(sl), std::end(sl));
  }
  SECTION("Pipe") {
    auto sl = ns | slice(2, 8, 2);
    v.assign(std::begin(sl), std::end(sl));
  }

  Vec vc = {12, 14, 16};
  REQUIRE(v == vc);
}

TEST_CASE("slice: empty iterable", "[slice]") {
  Vec ns{};
  auto sl = slice(ns, 3);
  REQUIRE(std::begin(sl) == std::end(sl));
}

TEST_CASE("slice: stop is beyond end of iterable", "[slice]") {
  Vec ns = {1, 2, 3};
  auto sl = slice(ns, 10);

  Vec v(std::begin(sl), std::end(sl));
  REQUIRE(v == ns);
}

TEST_CASE("slice: start is beyond end of iterable", "[slice]") {
  Vec ns = {1, 2, 3};
  auto sl = slice(ns, 5, 10);
  REQUIRE(std::begin(sl) == std::end(sl));
}

TEST_CASE("slice: (stop - start) % step != 0", "[slice]") {
  Vec ns = {1, 2, 3, 4};
  auto sl = slice(ns, 0, 2, 3);
  Vec v(std::begin(sl), std::end(sl));
  Vec vc = {1};

  REQUIRE(v == vc);
}

TEST_CASE("slice: invalid ranges give 0 size slices", "[slice]") {
  Vec ns = {1, 2, 3};
  SECTION("negative step") {
    auto sl = slice(ns, 1, 10, -1);
    REQUIRE(std::begin(sl) == std::end(sl));
  }
  SECTION("stop < start") {
    auto sl = slice(ns, 2, 0, 3);
    REQUIRE(std::begin(sl) == std::end(sl));
  }
}

TEST_CASE("slice: moves rvalues and binds to lvalues", "[slice]") {
  itertest::BasicIterable<int> bi{1, 2, 3, 4};
  slice(bi, 1, 3);
  REQUIRE_FALSE(bi.was_moved_from());
  auto sl = slice(std::move(bi), 1, 3);
  REQUIRE(bi.was_moved_from());

  Vec v(std::begin(sl), std::end(sl));
  Vec vc = {2, 3};

  REQUIRE(v == vc);
}

TEST_CASE("slice: with iterable doesn't move or copy elems", "[slice]") {
  constexpr itertest::SolidInt arr[] = {{6}, {7}, {8}};
  for (auto&& i : slice(arr, 2)) {
    (void)i;
  }
}

TEST_CASE("slice: iterator meets requirements", "[slice]") {
  std::string s{"abcdef"};
  auto c = slice(s, 1, 3);
  REQUIRE(itertest::IsIterator<decltype(std::begin(c))>::value);
}

template <typename T>
using ImpT = decltype(slice(std::declval<T>(), 1));
TEST_CASE("slice: has correct ctor and assign ops", "[slice]") {
  REQUIRE(itertest::IsMoveConstructibleOnly<ImpT<std::string&>>::value);
  REQUIRE(itertest::IsMoveConstructibleOnly<ImpT<std::string>>::value);
}
