#include <internal/iteratoriterator.hpp>

#include <string>
#include <vector>
#include <utility>

#include "catch.hpp"

using iter::impl::IterIterWrapper;

TEST_CASE("Iterator over a vector of vector iterators", "[iteratoriterator]") {
  using std::vector;
  vector<int> v = {2, 4, 6, 8};

  IterIterWrapper<vector<vector<int>::iterator>> itr;
  itr.get().push_back(std::begin(v) + 1);
  itr.get().push_back(std::end(v) - 1);
  itr.get().push_back(std::begin(v));

  auto it = std::begin(itr);
  REQUIRE(*it == 4);
  REQUIRE(it != std::end(itr));
  ++it;
  REQUIRE(*it == 8);
  it++;
  REQUIRE(*it == 2);
  ++it;
  REQUIRE(it == std::end(itr));

  REQUIRE(itr[0] == 4);
  REQUIRE(itr[1] == 8);
  REQUIRE(itr[2] == 2);

  auto rit = itr.rbegin();

  REQUIRE(*rit == 2);
  REQUIRE(rit != itr.rend());
  ++rit;
  REQUIRE(*rit == 8);
  ++rit;
  REQUIRE(*rit == 4);
  ++rit;
  REQUIRE(rit == itr.rend());
}

TEST_CASE("IteratorIterator operator->", "[iteratoriterator]") {
  using std::vector;
  using std::string;
  vector<string> v = {"hello", "everyone"};
  IterIterWrapper<vector<vector<string>::iterator>> itritr;
  itritr.get().push_back(std::end(v) - 1);
  itritr.get().push_back(std::begin(v));
  auto it = std::begin(itritr);
  REQUIRE(it->size() == 8);
}

TEST_CASE("Iterate over a vector of string iterators", "[iteratoriterator]") {
  std::string str = "hello world";
  IterIterWrapper<std::vector<std::string::iterator>> itritr;
  auto it = std::begin(itritr);
  static_assert(std::is_same<decltype(*it),
                    std::iterator_traits<decltype(it)>::reference>::value,
      "iterator is mis marked");
}

TEST_CASE("IteratorIterator supports mutable RandomAccessIterator operators",
    "[iteratoriterator]") {
  using std::vector;
  struct S {
    int value;
  };
  vector<S> v = {{2}, {4}, {6}, {8}};

  IterIterWrapper<vector<vector<S>::iterator>> itr;
  itr.get().push_back(std::begin(v) + 1);
  itr.get().push_back(std::end(v) - 1);
  itr.get().push_back(std::begin(v));

  // RandomAccessIterator (and ForwardIterator):
  auto a = itr.begin();
  auto r = a;
  auto r2 = r;
  ((r += 2) -= 2) += 2;
  REQUIRE(&(++r2) == &r2);  // Required by OutputIterator.
  REQUIRE(&(*r2++) == &a[1]);
  REQUIRE(r == r2);
  auto test_const_or_not = [&itr](auto& a, auto& b) {
    REQUIRE(!(b == a));
    REQUIRE(b == a + 2);
    REQUIRE(b == 2 + a);
    REQUIRE(b - 2 == a);
    REQUIRE(&a[2] == &b[0]);
    REQUIRE(b - a == 2);
    REQUIRE(a < b);
    REQUIRE(!(a < a));
    REQUIRE(b > a);
    REQUIRE(!(a > a));
    REQUIRE(a <= b);
    REQUIRE(!(b <= a));
    REQUIRE(a <= a);
    REQUIRE(b >= a);
    REQUIRE(!(a >= b));
    REQUIRE(a >= a);

    // InputIterator:
    REQUIRE(b != a);
    REQUIRE(!(a != a));
    REQUIRE(&(*a) != &(*b));
    REQUIRE(&(a->value) == &(*a).value);

    // Added methods, not from ...Iterator:
    REQUIRE(a.get() == std::begin(itr.get()));
  };
  test_const_or_not(a, r);
  test_const_or_not(std::as_const(a), r);
  test_const_or_not(a, std::as_const(r));
  test_const_or_not(std::as_const(a), std::as_const(r));

  // BidirectionalIterator (and RandomAccessIterator):
  REQUIRE((--r)-- == a + 1);
  REQUIRE(r == a);
  REQUIRE(&(*r2--) == &a[2]);

  // OutputIterator (and RandomAccessIterator):
  *r++ = {10};
  REQUIRE(r == a + 1);
  REQUIRE(v[1].value == 10);
  *++r = {12};
  REQUIRE(r == a + 2);
  REQUIRE(v[0].value == 12);
  *r = {14};
  REQUIRE(r == a + 2);
  REQUIRE(v[0].value == 14);
  a[1] = {16};
  REQUIRE(a == itr.begin());
  REQUIRE(v[3].value == 16);
}

TEST_CASE("IterIterWrapper supports several SequenceContainer methodes",
    "[iteratoriterator]") {
  using std::vector;
  vector<int> v = {2, 4, 6, 8};

  IterIterWrapper<vector<vector<int>::iterator>> itr;
  itr.get().push_back(std::begin(v) + 1);
  itr.get().push_back(std::end(v) - 1);

  auto test_const_or_not = [&v](auto& c) {
    REQUIRE(c.at(0) == 4);
    REQUIRE(c.at(1) == 8);
    REQUIRE(c[0] == 4);
    REQUIRE(c[1] == 8);
    REQUIRE(!c.empty());
    REQUIRE(c.size() == 2);
    REQUIRE(*c.begin() == 4);
    REQUIRE(*(c.end() - 1) == 8);
    REQUIRE(*c.cbegin() == 4);
    REQUIRE(*(c.cend() - 1) == 8);
    REQUIRE(*c.rbegin() == 8);
    REQUIRE(*(c.rend() - 1) == 4);
    REQUIRE(*c.crbegin() == 8);
    REQUIRE(*(c.crend() - 1) == 4);

    // Added methods, not from SequenceContainer:
    REQUIRE(c.get()[0] == std::begin(v) + 1);
  };
  test_const_or_not(itr);
  test_const_or_not(std::as_const(itr));
}
