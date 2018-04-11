#include <enumerate.hpp>

#include "helpers.hpp"

#include <iterator>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace Catch {
  template <typename A, typename B>
  std::string toString(const std::pair<A, B>& p) {
    std::ostringstream oss;
    oss << '{' << p.first << ", " << p.second << '}';
    return oss.str();
  }
}

#include "catch.hpp"

using Vec = std::vector<std::pair<std::size_t, char>>;
using iter::enumerate;

using itertest::BasicIterable;
using itertest::SolidInt;

TEST_CASE("Basic Functioning enumerate", "[enumerate]") {
  std::string str = "abc";
  auto e = enumerate(str);
  Vec v(std::begin(e), std::end(e));
  Vec vc{{0, 'a'}, {1, 'b'}, {2, 'c'}};

  REQUIRE(v == vc);
}

TEST_CASE("const enumerate", "[enumerate][const]") {
  Vec v;
  SECTION("lvalue") {
    std::string str = "abc";
    const auto e = enumerate(str);
    v.assign(std::begin(e), std::end(e));
  }
  SECTION("rvalue") {
    const auto e = enumerate(std::string("abc"));
    v.assign(std::begin(e), std::end(e));
  }
  SECTION("const lvalue") {
    const std::string str = "abc";
    const auto e = enumerate(str);
    v.assign(std::begin(e), std::end(e));
  }

  Vec vc{{0, 'a'}, {1, 'b'}, {2, 'c'}};

  REQUIRE(v == vc);
}

TEST_CASE("enumerate: can modify underlying sequence", "[enumerate]") {
  std::string s = "abc";
  for (auto&& [i, c] : enumerate(s)) {
    c = '-';
  }
  REQUIRE(s == "---");
}

TEST_CASE("enumerate: const iterators can be compared", "[enumerate][const]") {
  auto e = enumerate(std::string("hello"));
  const auto& ce = e;
  (void)(std::begin(e) == std::end(ce));
}

TEST_CASE("enumerate: has .index, .element, .first, and .second") {
  std::string s = "abc";
  auto e = enumerate(s);
  auto it = std::begin(e);
  REQUIRE(it->index == it->first);
  REQUIRE(&it->element == &it->second);
}

TEST_CASE("Empty enumerate", "[enumerate]") {
  std::string emp{};
  auto e = enumerate(emp);
  REQUIRE(std::begin(e) == std::end(e));
}

TEST_CASE("Postfix ++ enumerate", "[enumerate]") {
  std::string s{"amz"};
  auto e = enumerate(s);
  auto it = std::begin(e);
  it++;
  REQUIRE((*it).first == 1);
}

TEST_CASE("enumerate: structured bindings", "[enumerate]") {
  {
    std::string s{"amz"};
    auto e = enumerate(s);
    auto it = std::begin(e);
    REQUIRE(std::tuple_size<std::decay_t<decltype(*it)>>{} == 2);
    REQUIRE(std::get<0>(*it) == it->first);
  }

  Vec v;
  for (auto && [ i, c ] : enumerate(std::string{"xyz"})) {
    v.emplace_back(i, c);
  }
  const Vec vc{{0, 'x'}, {1, 'y'}, {2, 'z'}};
  REQUIRE(v == vc);
}

TEST_CASE("enumerate: with starting value", "[enumerate]") {
  std::string str = "hey";
  auto e = enumerate(str, 5u);
  Vec v(std::begin(e), std::end(e));
  Vec vc{{5, 'h'}, {6, 'e'}, {7, 'y'}};

  REQUIRE(v == vc);
}

TEST_CASE("Modifications through enumerate affect container", "[enumerate]") {
  std::vector<int> v{1, 2, 3, 4};
  std::vector<int> vc(v.size(), -1);
  for (auto&& p : enumerate(v)) {
    p.second = -1;
  }

  REQUIRE(v == vc);
}

TEST_CASE("enumerate with static array works", "[enumerate]") {
  char arr[] = {'w', 'x', 'y'};

  SECTION("Conversion to vector") {
    auto e = enumerate(arr);
    Vec v(std::begin(e), std::end(e));
    Vec vc{{0, 'w'}, {1, 'x'}, {2, 'y'}};
    REQUIRE(v == vc);
  }

  SECTION("Modification through enumerate") {
    for (auto&& p : enumerate(arr)) {
      p.second = 'z';
    }
    std::vector<char> v(std::begin(arr), std::end(arr));
    decltype(v) vc(v.size(), 'z');
    REQUIRE(v == vc);
  }
}

TEST_CASE("binds reference when it should", "[enumerate]") {
  BasicIterable<char> bi{'x', 'y', 'z'};
  auto e = enumerate(bi);
  (void)e;
  REQUIRE_FALSE(bi.was_moved_from());
}

TEST_CASE("moves rvalues into enumerable object", "[enumerate]") {
  BasicIterable<char> bi{'x', 'y', 'z'};
  auto e = enumerate(std::move(bi));
  REQUIRE(bi.was_moved_from());
  (void)e;
}

TEST_CASE("enumerate: operator->", "[enumerate]") {
  std::vector<int> ns = {50, 60, 70};
  auto e = enumerate(ns);
  auto it = std::begin(e);
  REQUIRE(it->first == 0);
  REQUIRE(it->second == 50);
}

TEST_CASE("enumerate: index and element", "[enumerate]") {
  std::string s{"ace"};
  auto e = enumerate(s);
  auto it = std::begin(e);
  REQUIRE((*it).index == 0);
  REQUIRE((*it).element == 'a');

  Vec v;
  for (auto&& p : enumerate(s)) {
    v.emplace_back(p.index, p.element);
  }
  Vec vc{{0, 'a'}, {1, 'c'}, {2, 'e'}};
  REQUIRE(v == vc);
}

TEST_CASE("enumerate: index and element through arrow", "[enumerate]") {
  std::string s{"ace"};
  auto e = enumerate(s);
  SECTION("One inspection") {
    auto it = std::begin(e);
    REQUIRE(it->index == 0);
    REQUIRE(it->element == 'a');
  }

  SECTION("full loop") {
    Vec v;
    for (auto it = std::begin(e), end_it = std::end(e); it != end_it; ++it) {
      v.emplace_back(it->index, it->element);
    }
    Vec vc{{0, 'a'}, {1, 'c'}, {2, 'e'}};
    REQUIRE(v == vc);
  }
}

TEST_CASE("Works with const iterable", "[enumerate]") {
  const std::string s{"ace"};
  auto e = enumerate(s);
  Vec v(std::begin(e), std::end(e));
  Vec vc{{0, 'a'}, {1, 'c'}, {2, 'e'}};
  REQUIRE(v == vc);
}

TEST_CASE("Doesn't move or copy elements of iterable", "[enumerate]") {
  constexpr SolidInt arr[] = {{6}, {7}, {8}};
  for (auto&& i : enumerate(arr)) {
    (void)i;
  }
}

TEST_CASE("enumerate: iterator meets requirements", "[enumerate]") {
  std::string s{};
  auto c = enumerate(s);
  REQUIRE(itertest::IsIterator<decltype(std::begin(c))>::value);
}

TEST_CASE("enumerate: works with pipe", "[enumerate]") {
  constexpr char str[] = {'a', 'b', 'c'};
  auto e = str | enumerate;
  Vec v(std::begin(e), std::end(e));
  Vec vc{{0, 'a'}, {1, 'b'}, {2, 'c'}};

  REQUIRE(v == vc);
}

TEST_CASE("enumerate: works index and pipe", "[enumerate]") {
  constexpr char str[] = {'a', 'b', 'c'};
  auto e = str | enumerate(2);
  Vec v(std::begin(e), std::end(e));
  Vec vc{{2, 'a'}, {3, 'b'}, {4, 'c'}};

  REQUIRE(v == vc);
}

TEST_CASE(
    "enumerate: Works with different begin and end types", "[enumerate]") {
  CharRange cr{'d'};
  auto e = enumerate(cr);
  Vec v(e.begin(), e.end());
  Vec vc{{0, 'a'}, {1, 'b'}, {2, 'c'}};
  REQUIRE(v == vc);
}

template <typename T>
using ImpT = decltype(enumerate(std::declval<T>()));
TEST_CASE("enumerate: has correct ctor and assign ops", "[enumerate]") {
  REQUIRE(itertest::IsMoveConstructibleOnly<ImpT<std::string&>>::value);
  REQUIRE(itertest::IsMoveConstructibleOnly<ImpT<std::string>>::value);
}
