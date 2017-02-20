#include <chain.hpp>
#include "helpers.hpp"

#include <iterator>
#include <list>
#include <string>
#include <utility>
#include <vector>

#include "catch.hpp"

using iter::chain;
using itertest::SolidInt;
using itertest::BasicIterable;
using Vec = const std::vector<char>;

TEST_CASE("chain: three strings", "[chain]") {
  std::string s1{"abc"};
  std::string s2{"mno"};
  std::string s3{"xyz"};
  auto ch = chain(s1, s2, s3);

  Vec v(std::begin(ch), std::end(ch));
  Vec vc{'a', 'b', 'c', 'm', 'n', 'o', 'x', 'y', 'z'};

  REQUIRE(v == vc);
}

TEST_CASE("chain: with different container types", "[chain]") {
  std::string s1{"abc"};
  std::list<char> li{'m', 'n', 'o'};
  std::vector<char> vec{'x', 'y', 'z'};
  auto ch = chain(s1, li, vec);

  Vec v(std::begin(ch), std::end(ch));
  Vec vc{'a', 'b', 'c', 'm', 'n', 'o', 'x', 'y', 'z'};

  REQUIRE(v == vc);
}

TEST_CASE(
    "chain: where one container has different begin and end types", "[chain]") {
  std::string s1{"abc"};
  std::list<char> li{'m', 'n', 'o'};
  CharRange cr('e');
  auto ch = chain(s1, li, cr);

  Vec v(std::begin(ch), std::end(ch));
  Vec vc{'a', 'b', 'c', 'm', 'n', 'o', 'a', 'b', 'c', 'd'};

  REQUIRE(v == vc);
}

TEST_CASE("chain: handles empty containers", "[chain]") {
  std::string emp;
  std::string a{"a"};
  std::string b{"b"};
  std::string c{"c"};
  Vec vc{'a', 'b', 'c'};

  SECTION("Empty container at front") {
    auto ch = chain(emp, a, b, c);
    Vec v(std::begin(ch), std::end(ch));

    REQUIRE(v == vc);
  }

  SECTION("Empty container at back") {
    auto ch = chain(a, b, c, emp);
    Vec v(std::begin(ch), std::end(ch));

    REQUIRE(v == vc);
  }

  SECTION("Empty container in middle") {
    auto ch = chain(a, emp, b, emp, c);
    Vec v(std::begin(ch), std::end(ch));

    REQUIRE(v == vc);
  }

  SECTION("Consecutive empty containers at front") {
    auto ch = chain(emp, emp, a, b, c);
    Vec v(std::begin(ch), std::end(ch));

    REQUIRE(v == vc);
  }

  SECTION("Consecutive empty containers at back") {
    auto ch = chain(a, b, c, emp, emp);
    Vec v(std::begin(ch), std::end(ch));

    REQUIRE(v == vc);
  }

  SECTION("Consecutive empty containers in middle") {
    auto ch = chain(a, emp, emp, b, emp, emp, c);
    Vec v(std::begin(ch), std::end(ch));

    REQUIRE(v == vc);
  }
}

TEST_CASE("chain: with only empty containers", "[chain]") {
  std::string emp{};
  SECTION("one empty container") {
    auto ch = chain(emp);
    REQUIRE_FALSE(std::begin(ch) != std::end(ch));
  }

  SECTION("two empty containers") {
    auto ch = chain(emp, emp);
    REQUIRE_FALSE(std::begin(ch) != std::end(ch));
  }

  SECTION("three empty containers") {
    auto ch = chain(emp, emp, emp);
    REQUIRE_FALSE(std::begin(ch) != std::end(ch));
  }
}

TEST_CASE("chain: doesn't move or copy elements of iterable", "[chain]") {
  constexpr SolidInt arr[] = {{6}, {7}, {8}};
  for (auto&& i : chain(arr, arr)) {
    (void)i;
  }
}

TEST_CASE("chain: binds reference to lvalue and moves rvalue", "[chain]") {
  BasicIterable<char> bi{'x', 'y', 'z'};
  BasicIterable<char> bi2{'a', 'j', 'm'};
  SECTION("First moved, second ref'd") {
    chain(std::move(bi), bi2);
    REQUIRE(bi.was_moved_from());
    REQUIRE_FALSE(bi2.was_moved_from());
  }
  SECTION("First ref'd, second moved") {
    chain(bi, std::move(bi2));
    REQUIRE_FALSE(bi.was_moved_from());
    REQUIRE(bi2.was_moved_from());
  }
}

TEST_CASE("chain: operator==", "[chain]") {
  std::string emp{};
  auto ch = chain(emp);
  REQUIRE(std::begin(ch) == std::end(ch));
}

TEST_CASE("chain: postfix ++", "[chain]") {
  std::string s1{"a"}, s2{"b"};
  auto ch = chain(s1, s2);
  auto it = std::begin(ch);
  it++;
  REQUIRE(*it == 'b');
}

TEST_CASE("chain: iterator meets requirements", "[chain]") {
  Vec ns{};
  auto c = chain(ns, ns);
  REQUIRE(itertest::IsIterator<decltype(std::begin(c))>::value);
}

template <typename... Ts>
using ImpT = decltype(chain(std::declval<Ts>()...));
TEST_CASE("chain: has correct ctor and assign ops", "[chain]") {
  using T = ImpT<std::string&, std::vector<char>, char(&)[10]>;
  REQUIRE(itertest::IsMoveConstructibleOnly<T>::value);
}

TEST_CASE("chain.from_iterable: basic test", "[chain.from_iterable]") {
  std::vector<std::string> sv{"abc", "xyz"};
  std::vector<char> v;
  SECTION("Normal call") {
    auto ch = chain.from_iterable(sv);
    v.assign(std::begin(ch), std::end(ch));
  }
  SECTION("Pipe") {
    auto ch = sv | chain.from_iterable;
    v.assign(std::begin(ch), std::end(ch));
  }

  std::vector<char> vc{'a', 'b', 'c', 'x', 'y', 'z'};
  REQUIRE(v == vc);
}

TEST_CASE("chain.fromm_iterable: Works with different begin and end types",
    "[chain.from_iterable]") {
  std::vector<CharRange> crv = {{'c'}, {'d'}};
  auto ch = chain.from_iterable(crv);
  const std::vector<char> v(std::begin(ch), std::end(ch));
  const std::vector<char> vc = {'a', 'b', 'a', 'b', 'c'};
  REQUIRE(v == vc);
}

TEST_CASE(
    "chain.from_iterable: iterators cant be copy constructed "
    "and assigned",
    "[chain.from_iterable]") {
  std::vector<std::string> sv{"abc", "xyz"};
  auto ch = chain.from_iterable(sv);
  auto it = std::begin(ch);

  SECTION("Copy constructed") {
    auto it2 = it;
    ++it;
    REQUIRE(it != it2);
  }

  SECTION("Copy assigned") {
    auto it2 = std::end(ch);
    it2 = it;
    REQUIRE(it == it2);
  }
}

TEST_CASE("chain.from_iterable: postfix ++", "[chain.from_iterable]") {
  std::vector<std::string> sv{"a", "n"};
  auto ch = chain.from_iterable(sv);
  auto it = std::begin(ch);
  it++;
  REQUIRE(*it == 'n');
}

TEST_CASE("chain.from_iterable: operator->", "[chain.from_iterable]") {
  std::vector<std::vector<std::string>> sv{{"a", "ab"}, {"abc"}};
  auto ch = chain.from_iterable(sv);
  auto it = std::begin(ch);
  REQUIRE(it->size() == 1);
}

TEST_CASE("chain.from_iterable: moves rvalues and binds ref to lvalues",
    "[chain.from_iterable]") {
  BasicIterable<std::string> bi{"abc", "xyz"};
  SECTION("Moves rvalue") {
    chain.from_iterable(std::move(bi));
    REQUIRE(bi.was_moved_from());
  }
  SECTION("Binds ref to lvalue") {
    chain.from_iterable(bi);
    REQUIRE_FALSE(bi.was_moved_from());
  }
}

TEST_CASE("chain.from_iterable: empty", "[chain.from_iterable]") {
  const std::vector<std::string> v{};
  auto ch = chain.from_iterable(v);
  REQUIRE(std::begin(ch) == std::end(ch));
}

TEST_CASE("chain.from_iterable: iterator meets requirements",
    "[chain.from_iterable]") {
  const std::vector<std::string> v{};
  auto c = chain.from_iterable(v);
  REQUIRE(itertest::IsIterator<decltype(std::begin(c))>::value);
}

template <typename T>
using ImpT2 = decltype(chain.from_iterable(std::declval<T>()));
TEST_CASE("chain.from_iterable: has correct ctor and assign ops",
    "[chain.from_iterable]") {
  REQUIRE(itertest::IsMoveConstructibleOnly<ImpT2<std::vector<std::string>>>::
          value);
  REQUIRE(itertest::IsMoveConstructibleOnly<ImpT2<std::vector<std::string>&>>::
          value);
}
