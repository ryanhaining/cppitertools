#include <accumulate.hpp>
#include "helpers.hpp"

#include <iterator>
#include <string>
#include <vector>

#include "catch.hpp"

using iter::accumulate;
using itertest::BasicIterable;

using Vec = const std::vector<int>;
TEST_CASE("Simple sum", "[accumulate]") {
  Vec ns{1, 2, 3, 4, 5};

  std::vector<int> v;
  SECTION("Normal call") {
    auto a = accumulate(ns);
    v.assign(std::begin(a), std::end(a));
  }
  SECTION("Pipe") {
    auto a = ns | accumulate;
    v.assign(std::begin(a), std::end(a));
  }

  Vec vc{1, 3, 6, 10, 15};
  REQUIRE(v == vc);
}

TEST_CASE("accumulate: With subtraction lambda", "[accumulate]") {
  Vec ns{5, 4, 3, 2, 1};
  std::vector<int> v;

  SECTION("Normal call") {
    auto a = accumulate(ns, [](int a, int b) { return a - b; });
    v.assign(std::begin(a), std::end(a));
  }
  SECTION("Pipe") {
    auto a = ns | accumulate([](int a, int b) { return a - b; });
    v.assign(std::begin(a), std::end(a));
  }

  Vec vc{5, 1, -2, -4, -5};
  REQUIRE(v == vc);
}

struct Integer {
  const int value;
  constexpr Integer(int i) : value{i} {}
  constexpr Integer operator+(Integer other) const noexcept {
    return {this->value + other.value};
  }
};

TEST_CASE("accumulate: intermidate type need not be default constructible",
    "[accumulate]") {
  std::vector<Integer> v = {{2}, {3}, {10}};
  auto a = accumulate(v, std::plus<Integer>{});
  auto it = std::begin(a);
}

TEST_CASE("accumulate: binds reference when it should", "[accumulate]") {
  BasicIterable<int> bi{1, 2};
  accumulate(bi);
  REQUIRE_FALSE(bi.was_moved_from());
}

TEST_CASE("accumulate: moves rvalues when it should", "[accumulate]") {
  BasicIterable<int> bi{1, 2};
  accumulate(std::move(bi));
  REQUIRE(bi.was_moved_from());
}

TEST_CASE("accumulate: operator==", "[accumulate]") {
  Vec v;
  auto a = accumulate(v);
  REQUIRE(std::begin(a) == std::end(a));
}

TEST_CASE("accumulate: postfix ++", "[accumulate]") {
  Vec ns{2, 3};
  auto a = accumulate(ns);
  auto it = std::begin(a);
  it++;
  REQUIRE(*it == 5);
}

TEST_CASE("accumulate: operator->", "[accumulate]") {
  Vec ns{7, 3};
  auto a = accumulate(ns);
  auto it = std::begin(a);
  const int* p = it.operator->();
  REQUIRE(*p == 7);
}

TEST_CASE("accumulate: iterator meets requirements", "[accumulate]") {
  Vec ns{};
  auto a = accumulate(ns, [](int a, int b) { return a + b; });
  auto it = std::begin(a);
  it = std::begin(a);
  REQUIRE(itertest::IsIterator<decltype(std::begin(a))>::value);
}

TEST_CASE(
    "accumulate: Works with different begin and end types", "[accumulate]") {
  CharRange cr{'d'};
  auto a = accumulate(cr);
  Vec v(a.begin(), a.end());
  Vec vc{'a', 'a' + 'b', 'a' + 'b' + 'c'};
  REQUIRE(v == vc);
}

template <typename T>
using ImpT = decltype(accumulate(std::declval<T>()));
TEST_CASE("accumulate: has correct ctor and assign ops", "[accumulate]") {
  REQUIRE(itertest::IsMoveConstructibleOnly<ImpT<std::string&>>::value);
  REQUIRE(itertest::IsMoveConstructibleOnly<ImpT<std::string>>::value);
}
