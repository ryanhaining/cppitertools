#include <repeat.hpp>

#include "helpers.hpp"

#include <iterator>
#include <string>
#include <vector>

#include "catch.hpp"

using iter::repeat;

TEST_CASE("repeat: one argument keeps giving value back", "[repeat]") {
  auto r = repeat('a');
  auto it = std::begin(r);
  REQUIRE(*it == 'a');
  ++it;
  REQUIRE(*it == 'a');
  ++it;
  REQUIRE(*it == 'a');
  ++it;
  REQUIRE(*it == 'a');
  ++it;
  REQUIRE(*it == 'a');
}

TEST_CASE("repeat: can be used as constexpr", "[repeat]") {
  static constexpr char c = 'a';
  {
    constexpr auto r = repeat(c);
    constexpr auto i = r.begin();
    constexpr char c2 = *i;
    static_assert(c == c2, "repeat value not as expected");
    constexpr auto i2 = ++i;
    (void)i2;
  }

  {
    constexpr static auto r = repeat('a');
    constexpr auto i = r.begin();
    constexpr char c2 = *i;
    static_assert(c2 == 'a', "repeat value not as expected");
  }

  {
    constexpr auto r = repeat(c, 2);
    constexpr auto i = r.begin();
    constexpr char c2 = *i;
    static_assert(c2 == c, "repeat value not as expected");
  }
}

TEST_CASE("repeat: two argument repeats a number of times", "[repeat]") {
  auto r = repeat('a', 3);
  std::string s(std::begin(r), std::end(r));
  REQUIRE(s == "aaa");
}

TEST_CASE("repeat: 0 count gives empty sequence", "[repeat]") {
  auto r = repeat('a', 0);
  REQUIRE(std::begin(r) == std::end(r));
}

TEST_CASE("repeat: negative count gives empty sequence", "[repeat]") {
  auto r = repeat('a', -2);
  REQUIRE(std::begin(r) == std::end(r));
  auto r2 = repeat('a', -1);
  REQUIRE(std::begin(r2) == std::end(r2));
}

TEST_CASE("repeat: doesn't duplicate item", "[repeat]") {
  itertest::SolidInt si{2};
  auto r = repeat(si);
  auto it = std::begin(r);
  (void)*it;
}

TEST_CASE("repeat: iterator meets requirements", "[repeat]") {
  auto r = repeat(1);
  REQUIRE(itertest::IsIterator<decltype(std::begin(r))>::value);
}

template <typename T>
using ImpT = decltype(repeat(std::declval<T>()));

template <typename T>
using ImpT2 = decltype(repeat(std::declval<T>(), 1));

TEST_CASE("repeat: has correct ctor and assign ops", "[repeat]") {
  REQUIRE(itertest::IsMoveConstructibleOnly<ImpT<std::string&>>::value);
  REQUIRE(itertest::IsMoveConstructibleOnly<ImpT<std::string>>::value);
  REQUIRE(itertest::IsMoveConstructibleOnly<ImpT2<std::string&>>::value);
  REQUIRE(itertest::IsMoveConstructibleOnly<ImpT2<std::string>>::value);
}
