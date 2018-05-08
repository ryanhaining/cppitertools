#include <product.hpp>

#define DEFINE_BASIC_ITERABLE_COPY_CTOR
#define DEFINE_BASIC_ITERABLE_CONST_BEGIN_AND_END
#include "helpers.hpp"
#undef DEFINE_BASIC_ITERABLE_CONST_BEGIN_AND_END
#undef DEFINE_BASIC_ITERABLE_COPY_CTOR

#include <iterator>
#include <string>
#include <vector>

#include "catch.hpp"

using iter::product;
using Vec = const std::vector<int>;

TEST_CASE("product: basic test, two sequences", "[product]") {
  using TP = std::tuple<int, char>;
  using ResType = std::vector<TP>;

  Vec n1 = {0, 1};
  const std::string s{"abc"};

  auto p = product(n1, s);
  ResType v(std::begin(p), std::end(p));
  ResType vc = {
      TP{0, 'a'}, TP{0, 'b'}, TP{0, 'c'}, TP{1, 'a'}, TP{1, 'b'}, TP{1, 'c'}};

  REQUIRE(v == vc);
}

TEST_CASE("product: const iteration", "[product][const]") {
  using TP = std::tuple<int, char>;
  using ResType = std::vector<TP>;

  Vec n1 = {0, 1};
  const std::string s{"abc"};

  const auto p = product(n1, s);
  ResType v(std::begin(p), std::end(p));
  ResType vc = {
      TP{0, 'a'}, TP{0, 'b'}, TP{0, 'c'}, TP{1, 'a'}, TP{1, 'b'}, TP{1, 'c'}};

  REQUIRE(v == vc);
}

TEST_CASE("product: const iterators can be compared to non-const iterators",
    "[product][const]") {
  std::string s;
  auto p = product(Vec{}, s);
  const auto& cp = p;
  (void)(std::begin(p) == std::end(cp));
}

TEST_CASE("product: two sequences where one has different begin and end",
    "[product]") {
  using TP = std::tuple<int, char>;
  using ResType = std::vector<TP>;

  Vec n1 = {0, 1};
  CharRange cr('d');

  auto p = product(n1, cr);
  ResType v(std::begin(p), std::end(p));
  ResType vc = {
      TP{0, 'a'}, TP{0, 'b'}, TP{0, 'c'}, TP{1, 'a'}, TP{1, 'b'}, TP{1, 'c'}};

  REQUIRE(v == vc);
}

TEST_CASE("product: three sequences", "[product]") {
  using TP = std::tuple<int, char, int>;
  using ResType = const std::vector<TP>;

  Vec n1 = {0, 1};
  const std::string s{"ab"};
  Vec n2 = {2};

  auto p = product(n1, s, n2);
  ResType v(std::begin(p), std::end(p));

  ResType vc = {TP{0, 'a', 2}, TP{0, 'b', 2}, TP{1, 'a', 2}, TP{1, 'b', 2}};

  REQUIRE(v == vc);
}

TEST_CASE("product: with repeat", "[product]") {
  using TP = std::tuple<char, char, char>;
  using ResType = const std::vector<TP>;
  const std::string s = "hop";
  auto p = product<3>(s);
  ResType v(std::begin(p), std::end(p));

  ResType vc = {
      TP{'h', 'h', 'h'},
      TP{'h', 'h', 'o'},
      TP{'h', 'h', 'p'},
      TP{'h', 'o', 'h'},
      TP{'h', 'o', 'o'},
      TP{'h', 'o', 'p'},
      TP{'h', 'p', 'h'},
      TP{'h', 'p', 'o'},
      TP{'h', 'p', 'p'},
      TP{'o', 'h', 'h'},
      TP{'o', 'h', 'o'},
      TP{'o', 'h', 'p'},
      TP{'o', 'o', 'h'},
      TP{'o', 'o', 'o'},
      TP{'o', 'o', 'p'},
      TP{'o', 'p', 'h'},
      TP{'o', 'p', 'o'},
      TP{'o', 'p', 'p'},
      TP{'p', 'h', 'h'},
      TP{'p', 'h', 'o'},
      TP{'p', 'h', 'p'},
      TP{'p', 'o', 'h'},
      TP{'p', 'o', 'o'},
      TP{'p', 'o', 'p'},
      TP{'p', 'p', 'h'},
      TP{'p', 'p', 'o'},
      TP{'p', 'p', 'p'},
  };
  REQUIRE(v == vc);
}

TEST_CASE("product: empty when any iterable is empty", "[product]") {
  Vec n1 = {0, 1};
  Vec n2 = {0, 1, 2};
  Vec emp = {};

  SECTION("first iterable is empty") {
    auto p = product(emp, n1, n2);
    REQUIRE(std::begin(p) == std::end(p));
  }

  SECTION("middle iterable is empty") {
    auto p = product(n1, emp, n2);
    REQUIRE(std::begin(p) == std::end(p));
  }

  SECTION("last iterable is empty") {
    auto p = product(n1, n2, emp);
    REQUIRE(std::begin(p) == std::end(p));
  }
}

TEST_CASE("product: single iterable", "[product]") {
  const std::string s{"ab"};
  using TP = std::tuple<char>;
  using ResType = const std::vector<TP>;

  auto p = product(s);
  ResType v(std::begin(p), std::end(p));
  ResType vc = {TP{'a'}, TP{'b'}};

  REQUIRE(v == vc);
}

TEST_CASE("product: no arguments gives one empty tuple", "[product") {
  auto p = product();
  auto it = std::begin(p);
  REQUIRE(it != std::end(p));
  REQUIRE(*it == std::make_tuple());
  ++it;
  REQUIRE(it == std::end(p));
}

TEST_CASE("product: binds to lvalues and moves rvalues", "[product]") {
  itertest::BasicIterable<char> bi{'x', 'y'};
  itertest::BasicIterable<int> bi2{0, 1};

  SECTION("First ref'd, second moved") {
    product(bi, std::move(bi2));
    REQUIRE_FALSE(bi.was_moved_from());
    REQUIRE_FALSE(bi.was_copied_from());
    REQUIRE(bi2.was_moved_from());
  }

  SECTION("First moved, second ref'd") {
    product(std::move(bi), bi2);
    REQUIRE(bi.was_moved_from());
    REQUIRE_FALSE(bi2.was_copied_from());
    REQUIRE_FALSE(bi2.was_moved_from());
  }

  SECTION("repeat, lvalue not moved or copied") {
    product<2>(bi);
    REQUIRE_FALSE(bi.was_moved_from());
    REQUIRE_FALSE(bi.was_copied_from());
  }

  SECTION("repeat, const lvalue not moved or copied") {
    const auto& r = bi;
    product<2>(r);
    REQUIRE_FALSE(bi.was_moved_from());
    REQUIRE_FALSE(bi.was_copied_from());
  }

  SECTION("repeat, rvalue copied") {
    product<2>(std::move(bi));
    REQUIRE_FALSE(bi.was_moved_from());
    REQUIRE(bi.was_copied_from());
  }
}

TEST_CASE("product: handles a lot of containers values", "[product]") {
  constexpr char str[] = "";
  auto p = product<50>(str);
  p.begin();
}

TEST_CASE("product: doesn't move or copy elements of iterable", "[product]") {
  constexpr itertest::SolidInt arr[] = {{1}, {0}, {2}};
  for (auto&& t : product(arr)) {
    (void)std::get<0>(t);
  }
}

TEST_CASE("product: iterator meets requirements", "[product]") {
  std::string s{"abc"};
  auto c = product(s, s);
  REQUIRE(itertest::IsIterator<decltype(std::begin(c))>::value);
}

template <typename... Ts>
using ImpT = decltype(product(std::declval<Ts>()...));
TEST_CASE("product: has correct ctor and assign ops", "[product]") {
  using T = ImpT<std::string&, std::vector<double>, std::vector<std::string>>;
  REQUIRE(itertest::IsMoveConstructibleOnly<T>::value);
}
