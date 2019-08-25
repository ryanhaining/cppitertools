// mixing different itertools, there is nothing called iter::mixed()

#include "itertools.hpp"

#include "catch.hpp"

#include <iostream>
#include <vector>

class MyUnMovable {
  int val;

 public:
  constexpr MyUnMovable(int val) : val{val} {}

  MyUnMovable(const MyUnMovable&) = delete;
  MyUnMovable& operator=(const MyUnMovable&) = delete;

  MyUnMovable(MyUnMovable&& other) : val{other.val} {}

  constexpr int get_val() const {
    return val;
  }
  void set_val(int new_val) {
    this->val = new_val;
  }

  bool operator==(const MyUnMovable& other) const {
    return this->val == other.val;
  }

  bool operator!=(const MyUnMovable& other) const {
    return !(*this == other);
  }
};

namespace {
  auto inc_ten = [](MyUnMovable& el) -> MyUnMovable& {
    int va = el.get_val();
    el.set_val(va + 10);
    return el;
  };
  auto dec_ten = [](MyUnMovable& el) -> MyUnMovable& {
    int va = el.get_val();
    el.set_val(va - 10);
    return el;
  };
}

TEST_CASE("filtering doesn't dereference multiple times", "[imap][filter]") {
  using iter::filter;
  using iter::imap;

  // source data
  std::array<MyUnMovable, 3> arr = {{{41}, {42}, {43}}};

  auto transformed1 = imap(inc_ten, arr);
  auto filtered = filter(
      [](const MyUnMovable& el) { return 52 != el.get_val(); }, transformed1);
  auto transformed2 = imap(dec_ten, filtered);

  std::vector<int> v;
  for (auto&& el : transformed2) {
    // I would use imap again instead of the loop if this wasn't an imap
    // test
    v.push_back(el.get_val());
  }

  std::vector<int> vc = {41, 43};

  REQUIRE(v == vc);

  constexpr std::array<MyUnMovable, 3> arrc = {{{41}, {52}, {43}}};
  REQUIRE(arr == arrc);
}

TEST_CASE("dropwhile doesn't dereference multiple times", "[imap][dropwhile]") {
  using iter::dropwhile;
  using iter::imap;

  std::array<MyUnMovable, 3> arr = {{{41}, {42}, {43}}};

  auto transformed1 = imap(inc_ten, arr);
  auto filtered = dropwhile(
      [](const MyUnMovable& el) { return 52 != el.get_val(); }, transformed1);
  auto transformed2 = imap(dec_ten, filtered);

  std::vector<int> v;
  for (auto&& el : transformed2) {
    v.push_back(el.get_val());
  }

  std::vector<int> vc = {42, 43};

  std::vector<int> vsc = {51, 42, 43};
  auto get_vals = imap([](const MyUnMovable& mv) { return mv.get_val(); }, arr);
  std::vector<int> vs(std::begin(get_vals), std::end(get_vals));
  REQUIRE(vs == vsc);
}

TEST_CASE("takewhile doesn't dereference multiple times", "[imap][takewhile]") {
  using iter::imap;
  using iter::takewhile;

  std::array<MyUnMovable, 3> arr = {{{41}, {42}, {43}}};

  auto transformed1 = imap(inc_ten, arr);
  auto filtered = takewhile(
      [](const MyUnMovable& el) { return 53 != el.get_val(); }, transformed1);
  auto transformed2 = imap(dec_ten, filtered);

  std::vector<int> v;
  for (auto&& el : transformed2) {
    v.push_back(el.get_val());
  }

  std::vector<int> vc = {41, 42};

  std::vector<int> vsc = {41, 42, 53};
  auto get_vals = imap([](const MyUnMovable& mv) { return mv.get_val(); }, arr);
  std::vector<int> vs(std::begin(get_vals), std::end(get_vals));
  REQUIRE(vs == vsc);
}

TEST_CASE("sorted(chain.from_iterable)", "[sorted][chain.from_iterable]") {
  std::vector<std::vector<int>> v = {{2, 4, 6}};
  auto s = iter::sorted(iter::chain.from_iterable(v));
}

TEST_CASE("filter into enumerate with pipe", "[filter][enumerate]") {
  using iter::enumerate;
  using iter::filter;
  using iter::imap;

  std::array<MyUnMovable, 4> arr = {{{41}, {42}, {43}, {44}}};
  auto seq =
      arr | filter([](const MyUnMovable& mv) { return mv.get_val() % 2 == 0; })
      | enumerate | imap([](const auto& imv) {
          return std::make_pair(imv.first, imv.second.get_val());
        });
  using Vec = std::vector<std::pair<std::size_t, int>>;
  const Vec v(std::begin(seq), std::end(seq));
  const Vec vc = {{0, 42}, {1, 44}};
  REQUIRE(v == vc);
}

TEST_CASE("enumerate(filter(chunked()))", "[filter][enumerate][chunked]") {
  using iter::chunked;
  using iter::enumerate;
  using iter::filter;
  std::vector<int16_t> v(500);
  auto chunks = chunked(v, 100);
  auto filtered = filter([](auto&) { return true; }, chunks);
  for (auto&& [i, chunk] : enumerate(filtered)) {
    (void)i;
    REQUIRE(chunk.size() == 100);
  }
}

TEST_CASE("zip(filter(chunked()))", "[filter][chunked][zip]") {
  using iter::chunked;
  using iter::filter;
  using iter::zip;
  std::vector<int16_t> v(500);
  auto chunks = chunked(v, 100);
  auto filtered = filter([](auto&) { return true; }, chunks);
  for (auto&& [chunk] : zip(filtered)) {
    REQUIRE(chunk.size() == 100);
  }
}

TEST_CASE("zip(filter(sliding_window()))", "[filter][sliding_window][zip]") {
  using iter::filter;
  using iter::sliding_window;
  using iter::zip;
  std::vector<int16_t> v(15);
  auto windows = sliding_window(v, 10);
  auto filtered = filter([](auto&) { return true; }, windows);
  for (auto&& [window] : zip(filtered)) {
    REQUIRE(window.size() == 10);
  }
}

TEST_CASE("imap(filter(groupby()))", "[filter][groupby][imap])") {
  using iter::filter;
  using iter::groupby;
  using iter::imap;

  std::vector<int> v{true, true, true, false, false, true, true};
  auto a = groupby(v, [](bool b) { return b; });
  auto b = filter([](auto& g) { return g.first; }, a);
  auto c = imap(
      [](auto& g) { return std::distance(g.second.begin(), g.second.end()); },
      b);
  for (auto x : c) {
    (void)x;
  }
}

TEST_CASE("imap(dropwhile(groupby()))", "[dropwhile][groupby][imap])") {
  using iter::dropwhile;
  using iter::groupby;
  using iter::imap;

  std::vector<int> v{false, false, true, true, false, true, true};
  auto a = groupby(v, [](bool b) { return b; });
  auto b = dropwhile([](auto& g) { return g.first; }, a);
  auto c = imap(
      [](auto& g) { return std::distance(g.second.begin(), g.second.end()); },
      b);
  for (auto x : c) {
    (void)x;
  }
}

TEST_CASE("imap(takewhile(groupby()))", "[takewhile][groupby][imap])") {
  using iter::groupby;
  using iter::imap;
  using iter::takewhile;

  std::vector<int> v{true, true, true, false, false};
  auto a = groupby(v, [](bool b) { return b; });
  auto b = takewhile([](auto& g) { return g.first; }, a);
  auto c = imap(
      [](auto& g) { return std::distance(g.second.begin(), g.second.end()); },
      b);
  for (auto x : c) {
    (void)x;
  }
}

TEST_CASE("chain.from_iterable: accept imap result that yields rvalues",
    "[chain.from_iterable][imap]") {
  using iter::chain;
  using iter::imap;
  const std::vector<std::vector<char>> ns = {{'a'}, {'q'}, {'x', 'z'}};
  auto ch = iter::chain.from_iterable(iter::imap([](auto v) { return v; }, ns));
  const std::vector<char> v(std::begin(ch), std::end(ch));

  const std::vector<char> vc = {'a', 'q', 'x', 'z'};

  REQUIRE(v == vc);
}

TEST_CASE(
    "filter(enumerate()), DerefHolder works correctly (see github issue #62",
    "[filter][enumerate]") {
  using iter::enumerate;
  using iter::filter;
  std::vector<int> ns = {50, 55, 60, 65};
  auto f =
      iter::filter([](auto& i) { return std::get<0>(i) > 1; }, enumerate(ns));
  const std::vector<std::pair<int, int>> v(std::begin(f), std::end(f));

  const std::vector<std::pair<int, int>> vc = {{2, 60}, {3, 65}};

  REQUIRE(v == vc);
}
