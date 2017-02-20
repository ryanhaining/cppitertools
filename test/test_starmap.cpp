#include <imap.hpp>

#include "helpers.hpp"

#include <iterator>
#include <list>
#include <string>
#include <vector>

#include "catch.hpp"

using iter::starmap;

namespace {
  long f(long d, int i) {
    return d * i;
  }

  std::string g(const std::string& s, int i, char c) {
    std::stringstream ss;
    ss << s << ' ' << i << ' ' << c;
    return ss.str();
  }

  struct Callable {
    int operator()(int a, int b, int c) {
      return a + b + c;
    }

    int operator()(int a) {
      return a;
    }
  };
}

TEST_CASE("starmap: works with function pointer and lambda", "[starmap]") {
  using Vec = const std::vector<int>;
  const std::vector<std::pair<double, int>> v1 = {{1l, 2}, {3l, 11}, {6l, 7}};
  Vec vc = {2l, 33l, 42l};

  std::vector<int> v;
  SECTION("with function") {
    SECTION("Normal call") {
      auto sm = starmap(f, v1);
      v.assign(std::begin(sm), std::end(sm));
    }
    SECTION("pipe") {
      auto sm = v1 | starmap(f);
      v.assign(std::begin(sm), std::end(sm));
    }
  }

  SECTION("with lambda") {
    auto sm = starmap([](long a, int b) { return a * b; }, v1);
    v.assign(std::begin(sm), std::end(sm));
  }
  REQUIRE(v == vc);
}

TEST_CASE("starmap: Works with different begin and end types", "[starmap]") {
  IntCharPairRange icr{{3, 'd'}};
  using Vec = std::vector<std::string>;
  auto sm = starmap([](int i, char c) { return std::to_string(i) + c; }, icr);
  Vec v(sm.begin(), sm.end());
  Vec vc{"0a", "1b", "2c"};
  REQUIRE(v == vc);
}

TEST_CASE("starmap: list of tuples", "[starmap]") {
  using Vec = const std::vector<std::string>;
  using T = std::tuple<std::string, int, double>;
  std::list<T> li = {T{"hey", 42, 'a'}, T{"there", 3, 'b'}, T{"yall", 5, 'c'}};

  auto sm = starmap(g, li);
  Vec v(std::begin(sm), std::end(sm));
  Vec vc = {"hey 42 a", "there 3 b", "yall 5 c"};

  REQUIRE(v == vc);
}

TEST_CASE("starmap: tuple of tuples", "[starmap]") {
  using Vec = const std::vector<int>;
  auto tup = std::make_tuple(std::make_tuple(10, 19, 60), std::make_tuple(7));
  std::vector<int> v;
  SECTION("Normal call") {
    auto sm = starmap(Callable{}, tup);
    v.assign(std::begin(sm), std::end(sm));
  }
  SECTION("Pipe") {
    auto sm = tup | starmap(Callable{});
    v.assign(std::begin(sm), std::end(sm));
  }

  Vec vc = {89, 7};
  REQUIRE(v == vc);
}

TEST_CASE("starmap: tuple of pairs", "[starmap]") {
  using Vec = const std::vector<int>;
  auto p =
      std::make_pair(std::array<int, 3>{{15, 100, 2000}}, std::make_tuple(16));
  Callable c;
  auto sm = starmap(c, p);

  Vec v(std::begin(sm), std::end(sm));
  Vec vc = {2115, 16};

  REQUIRE(v == vc);
}

TEST_CASE("starmap: moves rvalues, binds to lvalues", "[starmap]") {
  itertest::BasicIterable<std::tuple<int>> bi{};
  starmap(Callable{}, bi);
  REQUIRE_FALSE(bi.was_moved_from());
  starmap(Callable{}, std::move(bi));
  REQUIRE(bi.was_moved_from());
}

TEST_CASE("starmap: iterator meets requirements", "[starmap]") {
  std::string s{};
  const std::vector<std::pair<double, int>> v1;
  auto sm = starmap([](long a, int b) { return a * b; }, v1);
  REQUIRE(itertest::IsIterator<decltype(std::begin(sm))>::value);
}

TEST_CASE(
    "starmap:  tuple of tuples iterator meets requirements", "[starmap]") {
  auto tup = std::make_tuple(std::make_tuple(10, 19, 60), std::make_tuple(7));
  auto sm = starmap(Callable{}, tup);
  REQUIRE(itertest::IsIterator<decltype(std::begin(sm))>::value);
}
