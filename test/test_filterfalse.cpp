#include <filterfalse.hpp>

#include "helpers.hpp"

#include <iterator>
#include <string>
#include <vector>

#include "catch.hpp"

using iter::filterfalse;

using Vec = const std::vector<int>;

namespace {
  bool less_than_five(int i) {
    return i < 5;
  }

  class LessThanValue {
   private:
    int compare_val;

   public:
    LessThanValue(int v) : compare_val(v) {}

    bool operator()(int i) {
      return i < this->compare_val;
    }
  };
}

TEST_CASE("filterfalse: handles different callable types", "[filterfalse]") {
  Vec ns = {1, 2, 5, 6, 3, 1, 7, -1, 5};
  Vec vc = {5, 6, 7, 5};
  SECTION("with function pointer") {
    auto f = filterfalse(less_than_five, ns);
    Vec v(std::begin(f), std::end(f));
    REQUIRE(v == vc);
  }

  SECTION("with callable object") {
    std::vector<int> v;
    SECTION("Normal call") {
      auto f = filterfalse(LessThanValue{5}, ns);
      v.assign(std::begin(f), std::end(f));
    }
    SECTION("Pipe") {
      auto f = ns | filterfalse(LessThanValue{5});
      v.assign(std::begin(f), std::end(f));
    }
    REQUIRE(v == vc);
  }

  SECTION("with lambda") {
    auto ltf = [](int i) { return i < 5; };
    auto f = filterfalse(ltf, ns);
    Vec v(std::begin(f), std::end(f));
    REQUIRE(v == vc);
  }
}

TEST_CASE("filterfalse: handles pointer to member", "[filterfalse]") {
  using itertest::Point;
  const std::vector<Point> ps = {{0, 3}, {4, 0}, {0, 1}, {-1, -1}};
  std::vector<Point> v;
  SECTION("with pointer to data member") {
    auto f = filterfalse(&Point::x, ps);
    v.assign(std::begin(f), std::end(f));
  }

  SECTION("with pointer to member function") {
    auto f = filterfalse(&Point::get_x, ps);
    v.assign(std::begin(f), std::end(f));
  }

  const std::vector<Point> vc = {{0, 3}, {0, 1}};
  REQUIRE(v == vc);
}

TEST_CASE("filterfalse: const iteration", "[filterfalse][const]") {
  Vec ns = {1, 2, 5, 6, 3, 1, 7, -1, 5};
  const auto f = filterfalse(LessThanValue{5}, ns);
  Vec v(std::begin(f), std::end(f));
  Vec vc = {5, 6, 7, 5};
  REQUIRE(v == vc);
}

TEST_CASE("filterfalse: const iterator and non-const iterator can be compared",
    "[filterfalse][const]") {
  auto f = filterfalse(LessThanValue{5}, Vec{});
  const auto& cf = f;
  (void)(std::begin(f) == std::end(cf));
}

TEST_CASE(
    "filterfalse: Works with different begin and end types", "[filterfalse]") {
  CharRange cr{'d'};
  auto f = filterfalse([](char c) { return c == 'b'; }, cr);
  Vec v(f.begin(), f.end());
  Vec vc{'a', 'c'};
  REQUIRE(v == vc);
}

TEST_CASE("filterfalse: using identity", "[filterfalse]") {
  Vec ns{0, 1, 2, 0, 3, 0, 0, 0, 4, 5, 0};
  std::vector<int> v;
  SECTION("Normal call") {
    auto f = filterfalse(ns);
    v.assign(std::begin(f), std::end(f));
  }

  SECTION("Pipe") {
    auto f = ns | filterfalse;
    v.assign(std::begin(f), std::end(f));
  }

  Vec vc = {0, 0, 0, 0, 0, 0};
  REQUIRE(v == vc);
}

TEST_CASE("filterfalse: binds to lvalues, moves rvales", "[filterfalse]") {
  itertest::BasicIterable<int> bi{1, 2, 3, 4};

  SECTION("one-arg binds to lvalues") {
    filterfalse(bi);
    REQUIRE_FALSE(bi.was_moved_from());
  }

  SECTION("two-arg binds to lvalues") {
    filterfalse(less_than_five, bi);
    REQUIRE_FALSE(bi.was_moved_from());
  }

  SECTION("one-arg moves rvalues") {
    filterfalse(std::move(bi));
    REQUIRE(bi.was_moved_from());
  }

  SECTION("two-arg moves rvalues") {
    filterfalse(less_than_five, std::move(bi));
    REQUIRE(bi.was_moved_from());
  }
}

TEST_CASE("filterfalse: all elements pass predicate", "[filterfalse]") {
  Vec ns{0, 1, 2, 3, 4};
  auto f = filterfalse(less_than_five, ns);

  REQUIRE(std::begin(f) == std::end(f));
}

TEST_CASE("filterfalse: iterator meets requirements", "[filterfalse]") {
  std::string s{};
  auto c = filterfalse([] { return true; }, s);
  REQUIRE(itertest::IsIterator<decltype(std::begin(c))>::value);
}

TEST_CASE("filter: iterator is input if base is input", "[filter]") {
  using namespace itertest::archetypes;
  using C = Container<std::string, InputIterator>;
  C c(std::string{"abcdef"});
  auto f = filterfalse([](char c) { return c != 'a' && c != 'e'; }, c);
  using IterT = decltype(std::begin(f));
  REQUIRE(std::is_same<typename std::iterator_traits<IterT>::iterator_category,
      std::input_iterator_tag>::value);
  REQUIRE(!itertest::IsForwardIterator<IterT>::value);
}

TEST_CASE("filter: iterator is forward if base is forward", "[filter]") {
  using namespace itertest::archetypes;
  using C = Container<std::string, ForwardIterator>;
  C c(std::string{"abcdef"});
  auto f = filterfalse([](char c) { return c != 'a' && c != 'e'; }, c);
  using IterT = decltype(std::begin(f));
  REQUIRE(std::is_same<typename std::iterator_traits<IterT>::iterator_category,
      std::forward_iterator_tag>::value);
  REQUIRE(itertest::IsForwardIterator<IterT>::value);
}

template <typename T, typename U>
using ImpT = decltype(filterfalse(std::declval<T>(), std::declval<U>()));
TEST_CASE("filterfalse: has correct ctor and assign ops", "[filterfalse]") {
  using T1 = ImpT<bool (*)(char c), std::string&>;
  auto lam = [](char) { return false; };
  using T2 = ImpT<decltype(lam), std::string>;
  REQUIRE(itertest::IsMoveConstructibleOnly<T1>::value);
  REQUIRE(itertest::IsMoveConstructibleOnly<T2>::value);
}
