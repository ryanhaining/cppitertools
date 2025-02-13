#include <cppitertools/starmap.hpp>
#include "helpers.hpp"

#include <iterator>
#include <list>
#include <string>
#include <type_traits>
#include <vector>

#include "catch.hpp"

using iter::starmap;

namespace {
  long f(long d, int i) {
    return d * i;
  }

  int& larger_ref(int& a, int& b) {
    return a > b ? a : b;
  }

  const int& larger_const_ref(const int& a, const int& b) {
    return a > b ? a : b;
  }

  int larger(int a, int b) {
    return a > b ? a : b;
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

    int operator()(double a, int b) {
      return int(a + b);
    }

    int operator()(int a) {
      return a;
    }
  };

  struct Adder {
    long operator()(long a, int b) {
      return a + b;
    }
  };

  struct MoveOnlyAddAndPlus {
   private:
    // unique_ptr is better for triggering asan than an int if there's a
    // dangling reference to the callable
    std::unique_ptr<int> add_amount_;

   public:
    MoveOnlyAddAndPlus(int v) : add_amount_{std::make_unique<int>(v)} {}

    MoveOnlyAddAndPlus(const MoveOnlyAddAndPlus&) = delete;
    MoveOnlyAddAndPlus& operator=(const MoveOnlyAddAndPlus&) = delete;

    MoveOnlyAddAndPlus(MoveOnlyAddAndPlus&&) = default;
    MoveOnlyAddAndPlus& operator=(MoveOnlyAddAndPlus&&) = default;

    int operator()(long a, int b) {
      return a + b + *add_amount_;
    }
  };
}

TEST_CASE("starmap: works with function pointer and lambda", "[starmap]") {
  const std::vector<std::pair<long, int>> v1 = {{1l, 2}, {3l, 11}, {6l, 7}};
  const std::vector<int> greater_vc = {2l, 33l, 42l};
  const std::vector<long> added_vc = {3l, 14l, 13l};

  SECTION("with function pointer") {
    std::vector<int> v;
    SECTION("normal call") {
      auto sm = starmap(f, v1);
      v.assign(std::begin(sm), std::end(sm));
    }
    SECTION("pipe") {
      auto sm = v1 | starmap(f);
      v.assign(std::begin(sm), std::end(sm));
    }
    REQUIRE(v == greater_vc);
  }

  SECTION("with callable object") {
    std::vector<long> v;
    SECTION("normal call") {
      auto sm = starmap(Adder{}, v1);
      v.assign(std::begin(sm), std::end(sm));
    }
    SECTION("pipe") {
      auto sm = v1 | starmap(Adder{});
      v.assign(std::begin(sm), std::end(sm));
    }
    REQUIRE(v == added_vc);
  }

  SECTION("with lvalue callable object") {
    std::vector<long> v;
    auto adder = Adder{};
    SECTION("normal call") {
      auto sm = starmap(adder, v1);
      v.assign(std::begin(sm), std::end(sm));
    }
    SECTION("pipe") {
      auto sm = v1 | starmap(adder);
      v.assign(std::begin(sm), std::end(sm));
    }
    REQUIRE(v == std::vector{3l, 14l, 13l});
  }

  SECTION("with move-only callable object") {
    const std::vector<long> sum_plus_one_vc = {4l, 14l, 13l};
    std::vector<long> v;
    SECTION("normal call") {
      auto m = starmap(MoveOnlyAddAndPlus{1}, v1);
      v.assign(std::begin(m), std::end(m));
    }
    SECTION("pipe") {
      auto m = v1 | starmap(MoveOnlyAddAndPlus{1});
      v.assign(std::begin(m), std::end(m));
    }
  }

  SECTION("with lambda") {
    auto sm = starmap([](long a, int b) { return a * b; }, v1);
    std::vector<int> v(std::begin(sm), std::end(sm));
    REQUIRE(v == greater_vc);
  }
}

TEST_CASE("starmap: works with pointer to member function", "[starmap]") {
  using itertest::Point;
  std::vector<std::pair<Point, std::string>> tup = {
      {{10, 20}, "a"}, {{6, 8}, "point"}, {{3, 15}, "pos"}};
  auto sm = starmap(&Point::prefix, tup);
  std::vector<std::string> v(std::begin(sm), std::end(sm));
  const std::vector<std::string> vc = {
      "a(10, 20)", "point(6, 8)", "pos(3, 15)"};
  REQUIRE(v == vc);
}

TEST_CASE("starmap: vector of pairs const iteration", "[starmap][const]") {
  using Vec = const std::vector<int>;
  const std::vector<std::pair<double, int>> v1 = {
      {1.0, 2}, {3.0, 11}, {6.0, 7}};

  const auto sm = starmap(Callable{}, v1);
  std::vector<int> v(std::begin(sm), std::end(sm));
  Vec vc = {3, 14, 13};
  REQUIRE(v == vc);
}

TEST_CASE(
    "starmap: vector of pairs const iterators can be compared to non-const "
    "iterators",
    "[starmap][const]") {
  const std::vector<std::pair<double, int>> v1;
  auto sm = starmap(Callable{}, v1);
  const auto& csm = sm;
  (void)(std::begin(sm) == std::end(csm));
}

TEST_CASE("starmap: Works with different begin and end types", "[starmap]") {
  IntCharPairRange icr{{3, 'd'}};
  using Vec = std::vector<std::string>;
  auto sm = starmap([](int i, char c) { return std::to_string(i) + c; }, icr);
  Vec v(sm.begin(), sm.end());
  Vec vc{"0a", "1b", "2c"};
  REQUIRE(v == vc);
}

TEST_CASE("starmap: tuple of tuples const iteration", "[starmap][const]") {
  using Vec = const std::vector<int>;
  auto tup = std::make_tuple(std::make_tuple(10, 19, 60), std::make_tuple(7));
  const auto sm = starmap(Callable{}, tup);
  Vec v(std::begin(sm), std::end(sm));
}

TEST_CASE(
    "starmap: tuple of tuples const iterators can be compared to non-const "
    "iterator",
    "[starmap][const]") {
  auto tup = std::make_tuple(std::make_tuple(10, 19, 60), std::make_tuple(7));
  auto sm = starmap(Callable{}, tup);
  const auto& csm = sm;
  (void)(std::begin(sm) == std::end(csm));
  (void)(std::begin(csm) == std::end(sm));
}

TEST_CASE("starmap: list of tuples", "[starmap]") {
  using Vec = const std::vector<std::string>;
  using T = std::tuple<std::string, int, char>;
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
  auto sm = starmap([](double a, int b) { return a * b; }, v1);
  REQUIRE(itertest::IsIterator<decltype(std::begin(sm))>::value);
}

TEST_CASE(
    "starmap:  tuple of tuples iterator meets requirements", "[starmap]") {
  auto tup = std::make_tuple(std::make_tuple(10, 19, 60), std::make_tuple(7));
  auto sm = starmap(Callable{}, tup);
  REQUIRE(itertest::IsIterator<decltype(std::begin(sm))>::value);
}

TEST_CASE("starmap: iterator dereference type matches 'reference' type alias",
    "[starmap]") {
  std::vector<std::pair<int, int>> input;
  SECTION("with reference return type") {
    auto sm = iter::starmap(larger_ref, input);
    REQUIRE(itertest::ReferenceMatchesDeref<decltype(std::begin(sm))>::value);
  }
  SECTION("with const reference return type") {
    auto sm = iter::starmap(larger_const_ref, input);
    REQUIRE(itertest::ReferenceMatchesDeref<decltype(std::begin(sm))>::value);
  }
  SECTION("with value return type") {
    auto sm = iter::starmap(larger, input);
    REQUIRE(itertest::ReferenceMatchesDeref<decltype(std::begin(sm))>::value);
  }
}

TEST_CASE("starmap: iterator has correct 'value' type alias", "[starmap]") {
  std::vector<std::pair<int, int>> input;
  SECTION("with reference return type") {
    auto sm = iter::starmap(larger_ref, input);
    REQUIRE(std::is_same_v<int, decltype(sm.begin())::value_type>);
  }
  SECTION("with const reference return type") {
    auto sm = iter::starmap(larger_const_ref, input);
    REQUIRE(std::is_same_v<int, decltype(sm.begin())::value_type>);
  }
  SECTION("with value return type") {
    auto sm = iter::starmap(larger, input);
    REQUIRE(std::is_same_v<int, decltype(sm.begin())::value_type>);
  }
}
