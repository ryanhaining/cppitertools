#include <internal/iteratoriterator.hpp>

#include <string>
#include <vector>

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
