#include <cppitertools/filter.hpp>
#include <iterator>
#include <string>
#include <vector>

#include "catch.hpp"
#include "helpers.hpp"

using iter::filter;

using Vec = const std::vector<int>;

TEST_CASE("filter: handles different callable types", "[filter]") {
  Vec ns = {1, 2, 5, 6, 3, 1, 7, -1, 5};
  Vec vc = {1, 2, 3, 1, -1};
  std::vector<int> v;
  SECTION("with function pointer") {
    auto f = filter(less_than_five, ns);
    v = Vec(std::begin(f), std::end(f));
  }

  SECTION("with callable object") {
    auto f = filter(LessThanValue{5}, ns);
    v = Vec(std::begin(f), std::end(f));
  }

  SECTION("with lvalue callable object") {
    auto lt = LessThanValue{5};
    SECTION("normal call") {
      auto f = filter(lt, ns);
      v = Vec(std::begin(f), std::end(f));
    }
    SECTION("pipe") {
      auto f = ns | filter(lt);
      v = Vec(std::begin(f), std::end(f));
    }
  }

  SECTION("with move-only callable object") {
    SECTION("normal call") {
      auto f = filter(MoveOnlyLessThanValue{5}, ns);
      v = Vec(std::begin(f), std::end(f));
    }
    SECTION("pipe") {
      auto f = ns | filter(MoveOnlyLessThanValue{5});
      v = Vec(std::begin(f), std::end(f));
    }
  }

  SECTION("with lambda") {
    auto ltf = [](int i) { return i < 5; };
    auto f = filter(ltf, ns);
    v = Vec(std::begin(f), std::end(f));
  }
  REQUIRE(v == vc);
}

TEST_CASE("filter: handles pointer to member", "[filter]") {
  using itertest::Point;
  const std::vector<Point> ps = {{0, 3}, {4, 0}, {0, 1}, {-1, -1}};
  std::vector<Point> v;
  SECTION("with pointer to data member") {
    auto f = filter(&Point::x, ps);
    v.assign(std::begin(f), std::end(f));
  }

  SECTION("with pointer to member function") {
    auto f = filter(&Point::get_x, ps);
    v.assign(std::begin(f), std::end(f));
  }

  const std::vector<Point> vc = {{4, 0}, {-1, -1}};
  REQUIRE(v == vc);
}

TEST_CASE("filter: const iteration", "[filter][const]") {
  Vec ns = {1, 2, 5, 6, 3, 1, 7, -1, 5};
  const auto f = filter(LessThanValue{5}, ns);
  Vec v(std::begin(f), std::end(f));
  Vec vc = {1, 2, 3, 1, -1};
  REQUIRE(v == vc);
}

TEST_CASE("filter: const iterator can be compared to non-const iterator",
    "[filter][const]") {
  auto f = filter(LessThanValue{5}, Vec{});
  const auto& cf = f;
  (void)(std::begin(f) == std::end(cf));
}

TEST_CASE("filter: iterator with lambda can be assigned", "[filter]") {
  Vec ns{};
  auto ltf = [](int i) { return i < 5; };
  auto f = filter(ltf, ns);
  auto it = std::begin(f);
  it = std::begin(f);
}

TEST_CASE("filter: using identity", "[filter]") {
  Vec ns{0, 1, 2, 0, 3, 0, 0, 0, 4, 5, 0};
  auto f = filter(ns);
  Vec v(std::begin(f), std::end(f));

  Vec vc = {1, 2, 3, 4, 5};
  REQUIRE(v == vc);
}

TEST_CASE("filter: skips null pointers", "[filter]") {
  int a = 1;
  int b = 2;
  const std::vector<int*> ns = {0, &a, nullptr, nullptr, &b, nullptr};

  auto f = filter(ns);
  const std::vector<int*> v(std::begin(f), std::end(f));
  const std::vector<int*> vc = {&a, &b};
  REQUIRE(v == vc);
}

TEST_CASE("filter: binds to lvalues, moves rvales", "[filter]") {
  itertest::BasicIterable<int> bi{1, 2, 3, 4};

  SECTION("one-arg binds to lvalues") {
    filter(bi);
    REQUIRE_FALSE(bi.was_moved_from());
  }

  SECTION("two-arg binds to lvalues") {
    filter(less_than_five, bi);
    REQUIRE_FALSE(bi.was_moved_from());
  }

  SECTION("one-arg moves rvalues") {
    filter(std::move(bi));
    REQUIRE(bi.was_moved_from());
  }

  SECTION("two-arg moves rvalues") {
    filter(less_than_five, std::move(bi));
    REQUIRE(bi.was_moved_from());
  }
}

TEST_CASE("filter: operator->", "[filter]") {
  std::vector<std::string> vs = {"ab", "abc", "abcdef"};
  auto f = filter([](const std::string& str) { return str.size() > 4; }, vs);
  auto it = std::begin(f);
  REQUIRE(it->size() == 6);
}

TEST_CASE("filter: all elements fail predicate", "[filter]") {
  Vec ns{10, 20, 30, 40, 50};
  auto f = filter(less_than_five, ns);

  SECTION("normal compare") {
    REQUIRE(std::begin(f) == std::end(f));
  }
  SECTION("reversed compare") {
    REQUIRE(std::end(f) == std::begin(f));
  }
}

TEST_CASE("filter: doesn't move or copy elements of iterable", "[filter]") {
  constexpr itertest::SolidInt arr[] = {{1}, {0}, {2}};
  for (auto&& i :
      filter([](const itertest::SolidInt& si) { return si.getint(); }, arr)) {
    (void)i;
  }
}

TEST_CASE("filter: works with pipe", "[filter]") {
  Vec ns = {1, 2, 5, 6, 3, 1, 7, -1, 5};
  Vec vc = {1, 2, 3, 1, -1};

  auto f = ns | filter(LessThanValue{5});
  Vec v(std::begin(f), std::end(f));
  REQUIRE(v == vc);
}

TEST_CASE("filter: using identity and pipe", "[filter]") {
  Vec ns{0, 1, 2, 0, 3, 0, 0, 0, 4, 5, 0};
  auto f = ns | filter;
  Vec v(std::begin(f), std::end(f));

  Vec vc = {1, 2, 3, 4, 5};
  REQUIRE(v == vc);
}

TEST_CASE("filter: Works with different begin and end types", "[filter]") {
  CharRange cr{'d'};
  auto f = filter([](char c) { return c != 'b'; }, cr);
  Vec v(f.begin(), f.end());
  Vec vc{'a', 'c'};
  REQUIRE(v == vc);
}

TEST_CASE("filter: iterator meets requirements", "[filter]") {
  std::string s{};
  auto c = filter([] { return true; }, s);
  REQUIRE(itertest::IsIterator<decltype(std::begin(c))>::value);
}

template <typename T, typename U>
using ImpT = decltype(filter(std::declval<T>(), std::declval<U>()));
TEST_CASE("filter: has correct ctor and assign ops", "[filter]") {
  using T1 = ImpT<bool (*)(char c), std::string&>;
  auto lam = [](char) { return false; };
  using T2 = ImpT<decltype(lam), std::string>;
  REQUIRE(itertest::IsMoveConstructibleOnly<T1>::value);
  REQUIRE(itertest::IsMoveConstructibleOnly<T2>::value);
}
