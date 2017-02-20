#include <groupby.hpp>

#include "helpers.hpp"

#include <iterator>
#include <string>
#include <vector>

#include "catch.hpp"

using iter::groupby;

namespace {
  int length(const std::string& s) {
    return s.size();
  }

  struct Sizer {
    int operator()(const std::string& s) {
      return s.size();
    }
  };

  const std::vector<std::string> vec = {
      "hi", "ab", "ho", "abc", "def", "abcde", "efghi"};
}

TEST_CASE("groupby: works with lambda, callable, and function pointer") {
  std::vector<int> keys;
  std::vector<std::vector<std::string>> groups;

  SECTION("Function pointer") {
    SECTION("Normal call") {
      for (auto&& gb : groupby(vec, length)) {
        keys.push_back(gb.first);
        groups.emplace_back(std::begin(gb.second), std::end(gb.second));
      }
    }
    SECTION("Pipe") {
      for (auto&& gb : vec | groupby(length)) {
        keys.push_back(gb.first);
        groups.emplace_back(std::begin(gb.second), std::end(gb.second));
      }
    }
  }

  SECTION("Callable object") {
    for (auto&& gb : groupby(vec, Sizer{})) {
      keys.push_back(gb.first);
      groups.emplace_back(std::begin(gb.second), std::end(gb.second));
    }
  }

  SECTION("lambda function") {
    for (auto&& gb :
        groupby(vec, [](const std::string& s) { return s.size(); })) {
      keys.push_back(gb.first);
      groups.emplace_back(std::begin(gb.second), std::end(gb.second));
    }
  }

  const std::vector<int> kc = {2, 3, 5};
  REQUIRE(keys == kc);

  const std::vector<std::vector<std::string>> gc = {
      {"hi", "ab", "ho"}, {"abc", "def"}, {"abcde", "efghi"},
  };

  REQUIRE(groups == gc);
}

TEST_CASE("groupby: Works with different begin and end types", "[groupby]") {
  CharRange cr{'f'};
  std::vector<bool> keys;
  std::vector<std::vector<char>> groups;
  for (auto&& gb : groupby(cr, [](char c) { return c == 'c'; })) {
    keys.push_back(gb.first);
    groups.emplace_back(std::begin(gb.second), std::end(gb.second));
  }
  const std::vector<bool> kc = {false, true, false};
  const std::vector<std::vector<char>> gc = {{'a', 'b'}, {'c'}, {'d', 'e'}};
  REQUIRE(keys == kc);
  REQUIRE(groups == gc);
}

TEST_CASE("groupby: groups can be skipped completely", "[groupby]") {
  std::vector<int> keys;
  std::vector<std::vector<std::string>> groups;
  for (auto&& gb : groupby(vec, &length)) {
    if (gb.first == 3) {
      continue;
    }
    keys.push_back(gb.first);
    groups.emplace_back(std::begin(gb.second), std::end(gb.second));
  }

  const std::vector<int> kc = {2, 5};
  REQUIRE(keys == kc);

  const std::vector<std::vector<std::string>> gc = {
      {"hi", "ab", "ho"}, {"abcde", "efghi"},
  };

  REQUIRE(groups == gc);
}

TEST_CASE("groupby: groups can be skipped partially", "[groupby]") {
  std::vector<int> keys;
  std::vector<std::vector<std::string>> groups;
  for (auto&& gb : groupby(vec, &length)) {
    keys.push_back(gb.first);
    if (gb.first == 3) {
      std::vector<std::string> cut_short = {*std::begin(gb.second)};
      groups.push_back(cut_short);
    } else {
      groups.emplace_back(std::begin(gb.second), std::end(gb.second));
    }
  }

  const std::vector<int> kc = {2, 3, 5};
  REQUIRE(keys == kc);

  const std::vector<std::vector<std::string>> gc = {
      {"hi", "ab", "ho"}, {"abc"}, {"abcde", "efghi"},
  };

  REQUIRE(groups == gc);
}

TEST_CASE("groupby: single argument uses elements as keys", "[groupby]") {
  std::vector<int> ivec = {5, 5, 6, 6, 19, 19, 19, 19, 69, 0, 10, 10};
  std::vector<int> keys;
  std::vector<std::vector<int>> groups;
  SECTION("Normal call") {
    for (auto&& gb : groupby(ivec)) {
      keys.push_back(gb.first);
      groups.emplace_back(std::begin(gb.second), std::end(gb.second));
    }
  }
  SECTION("Pipe") {
    for (auto&& gb : ivec | groupby) {
      keys.push_back(gb.first);
      groups.emplace_back(std::begin(gb.second), std::end(gb.second));
    }
  }

  const std::vector<int> kc = {5, 6, 19, 69, 0, 10};
  REQUIRE(keys == kc);

  std::vector<std::vector<int>> gc = {
      {5, 5}, {6, 6}, {19, 19, 19, 19}, {69}, {0}, {10, 10},
  };

  REQUIRE(groups == gc);
}

TEST_CASE("groupby: empty iterable yields nothing", "[groupby]") {
  std::vector<int> ivec{};
  auto g = groupby(ivec);
  REQUIRE(std::begin(g) == std::end(g));
}

TEST_CASE("groupby: inner iterator (group) not used", "[groupby]") {
  std::vector<int> keys;
  for (auto&& gb : groupby(vec, length)) {
    keys.push_back(gb.first);
  }

  std::vector<int> kc = {2, 3, 5};
  REQUIRE(keys == kc);
}

TEST_CASE("groupby: doesn't double dereference", "[groupby]") {
  itertest::InputIterable seq;
  for (auto&& kg : groupby(seq, [](int i) { return i < 3; })) {
    for (auto&& e : kg.second) {
      (void)e;
    }
  }
}

TEST_CASE("grouby: iterator doesn't need to be dereferenced before advanced",
    "[groupby]") {
  std::vector<int> ns = {2, 4, 7};
  auto g = groupby(ns);
  auto it = std::begin(g);
  ++it;
  REQUIRE((*it).first == 4);
}

TEST_CASE("groupby: iterator can be dereferenced multiple times", "[groupby]") {
  std::vector<int> ns = {2, 4, 7};
  auto g = groupby(ns);
  auto it = std::begin(g);
  auto k1 = (*it).first;
  auto k2 = (*it).first;
  REQUIRE(k1 == k2);
}

TEST_CASE(
    "groupby: copy constructed iterators behave as expected", "[groupby]") {
  std::vector<int> ns = {2, 3, 4, 5};
  auto g = groupby(ns);
  auto it = std::begin(g);
  REQUIRE(it->first == 2);
  {
    auto it2 = it;
    REQUIRE(it2->first == 2);
    ++it;
    REQUIRE(it->first == 3);
    REQUIRE(*std::begin(it->second) == 3);
  }
  REQUIRE(it->first == 3);
  REQUIRE(*std::begin(it->second) == 3);
}

TEST_CASE("groupby: operator-> on both iterator types", "[groupby]") {
  std::vector<std::string> ns = {"a", "abc"};
  auto g = groupby(ns, [](const std::string& str) { return str.size(); });
  auto it = std::begin(g);
  REQUIRE(it->first == 1);
  auto it2 = std::begin(it->second);
  REQUIRE(it2->size() == 1);
}

TEST_CASE("groupby: iterator and groupiterator are correct", "[groupby]") {
  std::string s{"abc"};
  auto c = groupby(s);
  auto it = std::begin(c);
  REQUIRE(itertest::IsIterator<decltype(it)>::value);
  auto&& gp = (*it).second;
  auto it2 = std::begin(gp);
  REQUIRE(itertest::IsIterator<decltype(it2)>::value);
}

template <typename T, typename U>
using ImpT = decltype(groupby(std::declval<T>(), std::declval<U>()));
TEST_CASE("groupby: has correct ctor and assign ops", "[groupby]") {
  using T1 = ImpT<std::string&, bool (*)(char c)>;
  auto lam = [](char) { return false; };
  using T2 = ImpT<std::string, decltype(lam)>;
  REQUIRE(itertest::IsMoveConstructibleOnly<T1>::value);
  REQUIRE(itertest::IsMoveConstructibleOnly<T2>::value);
}
