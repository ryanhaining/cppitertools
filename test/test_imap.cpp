#include <cppitertools/imap.hpp>

#include "helpers.hpp"

#include <cctype>
#include <iterator>
#include <string>
#include <vector>

#include "catch.hpp"

using iter::imap;
using Vec = const std::vector<int>;

namespace {
  int plusone(int i) {
    return i + 1;
  }

  struct PlusOner {
    int operator()(int i) const {
      return i + 1;
    }
  };

  class MoveOnlyAdder {
   private:
    // unique_ptr is better for triggering asan than an int if there's a
    // dangling reference to the callable
    std::unique_ptr<int> add_amount_;

   public:
    MoveOnlyAdder(int v) : add_amount_{std::make_unique<int>(v)} {}

    MoveOnlyAdder(const MoveOnlyAdder&) = delete;
    MoveOnlyAdder& operator=(const MoveOnlyAdder&) = delete;

    MoveOnlyAdder(MoveOnlyAdder&&) = default;
    MoveOnlyAdder& operator=(MoveOnlyAdder&&) = default;

    int operator()(int i) {
      return i + *add_amount_;
    }
  };

  int power(int b, int e) {
    int acc = 1;
    for (int i = 0; i < e; ++i) {
      acc *= b;
    }
    return acc;
  }
}

TEST_CASE("imap: handles different callable types", "[imap]") {
  Vec ns = {10, 15, 300};
  Vec vc = {11, 16, 301};
  std::vector<int> v;
  SECTION("with function pointer") {
    auto m = imap(plusone, ns);
    v = Vec(std::begin(m), std::end(m));
  }

  SECTION("with callable object") {
    auto m = imap(PlusOner{}, ns);
    v = Vec(std::begin(m), std::end(m));
  }

  SECTION("with lvalue callable object") {
    auto lt = PlusOner{};
    SECTION("normal call") {
      auto m = imap(lt, ns);
      v = Vec(std::begin(m), std::end(m));
    }
    SECTION("pipe") {
      auto m = ns | imap(lt);
      v = Vec(std::begin(m), std::end(m));
    }
  }

  SECTION("with move-only callable object") {
    SECTION("normal call") {
      auto m = imap(MoveOnlyAdder{1}, ns);
      v = Vec(std::begin(m), std::end(m));
    }
    SECTION("pipe") {
      auto m = ns | imap(MoveOnlyAdder{1});
      v = Vec(std::begin(m), std::end(m));
    }
  }

  SECTION("with lambda") {
    auto ltf = [](int i) { return i + 1; };
    auto m = imap(ltf, ns);
    v = Vec(std::begin(m), std::end(m));
  }
  REQUIRE(v == vc);
}

TEST_CASE("imap: works with pointer to member", "[imap]") {
  using itertest::Point;
  std::vector<Point> ps = {{3, 6}, {20, 25}};
  std::vector<int> v;
  SECTION("with pointer to member function") {
    auto im = imap(&Point::get_y, ps);
    v.assign(std::begin(im), std::end(im));
  }

  SECTION("with pointer to data member") {
    auto im = imap(&Point::y, ps);
    v.assign(std::begin(im), std::end(im));
  }

  Vec vc = {6, 25};
  REQUIRE(v == vc);
}

TEST_CASE("imap: works with pointer to member function taking argument") {
  using itertest::Point;
  std::vector<Point> ps = {{10, 20}, {6, 8}, {3, 15}};
  std::vector<std::string> strs = {"a", "point", "pos"};

  auto im = imap(&Point::prefix, ps, strs);

  std::vector<std::string> v(std::begin(im), std::end(im));
  const std::vector<std::string> vc = {
      "a(10, 20)", "point(6, 8)", "pos(3, 15)"};

  REQUIRE(v == vc);
}

// TODO enable once zip supports const
#if 0
TEST_CASE("imap: supports const iteration", "[imap][const]") {
  Vec ns = {10, 20, 30};
  const auto m = imap(PlusOner{}, ns);
  Vec v(std::begin(m), std::end(m));
  Vec vc = {11, 21, 31};
  REQUIRE(v == vc);
}

TEST_CASE("imap: const iterators can be compared to non-const iterators", "[imap][const]") {
  auto m = imap(PlusOner{}, Vec{});
  const auto& cm = m;
  (void)(std::begin(m) == std::end(cm));
}
#endif

TEST_CASE("imap: Works with different begin and end types", "[imap]") {
  CharRange cr{'d'};
  auto m = imap([](char c) { return std::toupper(c); }, cr);
  Vec v(m.begin(), m.end());
  Vec vc{'A', 'B', 'C'};
  REQUIRE(v == vc);
}

TEST_CASE("imap: works with multiple sequences", "[imap]") {
  Vec bases = {0, 1, 2, 3};
  Vec exps = {1, 2, 3, 4};

  auto im = imap(power, bases, exps);
  Vec v(std::begin(im), std::end(im));
  Vec vc = {0, 1, 8, 81};

  REQUIRE(v == vc);
}

TEST_CASE("imap: terminates on shortest squence", "[imap]") {
  Vec ns1 = {1, 2, 3, 4};
  Vec ns2 = {2, 4, 6, 8, 10};
  Vec vc = {3, 6, 9, 12};
  SECTION("shortest sequence first") {
    auto im = imap([](int a, int b) { return a + b; }, ns1, ns2);
    Vec v(std::begin(im), std::end(im));
    REQUIRE(v == vc);
  }
  SECTION("shortest sequence second") {
    auto im = imap([](int a, int b) { return a + b; }, ns2, ns1);
    Vec v(std::begin(im), std::end(im));
    REQUIRE(v == vc);
  }
}

TEST_CASE("imap: operator->", "[imap]") {
  std::vector<std::string> vs = {"ab", "abcd", "abcdefg"};
  {
    auto m = imap([](std::string& s) { return s; }, vs);
    auto it = std::begin(m);
    REQUIRE(it->size() == 2);
  }

  {
    auto m = imap([](std::string& s) -> std::string& { return s; }, vs);
    auto it = std::begin(m);
    REQUIRE(it->size() == 2);
  }
}

TEST_CASE("imap: empty sequence gives nothing", "[imap]") {
  Vec v{};
  auto im = imap(plusone, v);
  REQUIRE(std::begin(im) == std::end(im));
}

TEST_CASE("imap: binds to lvalues, moves rvalues", "[imap]") {
  itertest::BasicIterable<int> bi{1, 2};
  SECTION("binds to lvalues") {
    imap(plusone, bi);
    REQUIRE_FALSE(bi.was_moved_from());
  }

  SECTION("moves rvalues") {
    imap(plusone, std::move(bi));
    REQUIRE(bi.was_moved_from());
  }
}

TEST_CASE("imap: doesn't move or copy elements of iterable", "[imap]") {
  constexpr itertest::SolidInt arr[] = {{1}, {0}, {2}};
  for (auto&& i :
      imap([](const itertest::SolidInt& si) { return si.getint(); }, arr)) {
    (void)i;
  }
}

TEST_CASE("imap: postfix ++", "[imap]") {
  Vec ns = {10, 20};
  auto im = imap(plusone, ns);
  auto it = std::begin(im);
  it++;
  REQUIRE((*it) == 21);
  it++;
  REQUIRE(it == std::end(im));
}

TEST_CASE("imap: iterator meets requirements", "[imap]") {
  std::string s{};
  auto c = imap([](char) { return 1; }, s);
  REQUIRE(itertest::IsIterator<decltype(std::begin(c))>::value);
}

template <typename T, typename U>
using ImpT = decltype(imap(std::declval<T>(), std::declval<U>()));
TEST_CASE("imap: has correct ctor and assign ops", "[imap]") {
  using T1 = ImpT<bool (*)(char c), std::string&>;
  auto lam = [](char) { return false; };
  using T2 = ImpT<decltype(lam), std::string>;
  REQUIRE(itertest::IsMoveConstructibleOnly<T1>::value);
  REQUIRE(itertest::IsMoveConstructibleOnly<T2>::value);
}
