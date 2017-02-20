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
  void set_val(int val) {
    this->val = val;
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
  using iter::imap;
  using iter::dropwhile;

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
  using iter::imap;
  using iter::filter;
  using iter::enumerate;

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
