#include <shuffled.hpp>

#include <set>
#include <unordered_set>
#include <vector>
#include <array>
#include <string>
#include <utility>
#include <list>

#include "helpers.hpp"
#include "catch.hpp"

using iter::shuffled;

using Vec = const std::vector<int>;

TEST_CASE("shuffled: iterates through a vector in shuffled order", "[shuffled]") {
  Vec ns = {4, 0, 5, 1, 6, 7, 9, 3, 2, 8};
  auto s = shuffled(ns);
  Vec v(std::begin(s), std::end(s));
  Vec vc = {2, 9, 8, 6, 7, 5, 3, 1, 0, 4};
  REQUIRE(v == vc);
}

TEST_CASE("shuffled: iterates through a list of strings in shuffled order",
          "[shuffled]") {
  std::list<std::string> ns = {"apple", "banana", "orange", "peach"};
  auto s = shuffled(ns);
  std::list<std::string> v(std::begin(s), std::end(s));
  std::list<std::string> vc = {"orange", "peach", "banana", "apple"};
  REQUIRE(v == vc);
}

TEST_CASE("shuffled: restore iteration through a vector in shuffled order "
          "from the index of the last seen element in non shuffled container",
          "[shuffled]") {
  //index:  0  1  2  3  4  5  6  7  8  9
  Vec ns = {4, 0, 5, 1, 6, 7, 9, 3, 2, 8};
  auto s = shuffled(ns);

  // overflow - same as default
  Vec v10(s.restore(10), std::end(s));
  Vec vc10 = {2, 9, 8, 6, 7, 5, 3, 1, 0, 4};
  REQUIRE(v10 == vc10);

  Vec v1(s.restore(6), std::end(s));
  Vec vc1 = {9, 8, 6, 7, 5, 3, 1, 0, 4};
  REQUIRE(v1 == vc1);

  Vec v2(s.restore(9), std::end(s));
  Vec vc2 = {8, 6, 7, 5, 3, 1, 0, 4};
  REQUIRE(v2 == vc2);

  Vec v3(s.restore(4), std::end(s));
  Vec vc3 = {6, 7, 5, 3, 1, 0, 4};
  REQUIRE(v3 == vc3);

  Vec v4(s.restore(5), std::end(s));
  Vec vc4 = {7, 5, 3, 1, 0, 4};
  REQUIRE(v4 == vc4);

  Vec v5(s.restore(2), std::end(s));
  Vec vc5 = {5, 3, 1, 0, 4};
  REQUIRE(v5 == vc5);

  Vec v6(s.restore(7), std::end(s));
  Vec vc6 = {3, 1, 0, 4};
  REQUIRE(v6 == vc6);

  Vec v7(s.restore(3), std::end(s));
  Vec vc7 = {1, 0, 4};
  REQUIRE(v7 == vc7);

  Vec v8(s.restore(1), std::end(s));
  Vec vc8 = {0, 4};
  REQUIRE(v8 == vc8);

  Vec v9(s.restore(0), std::end(s));
  Vec vc9 = {4};
  REQUIRE(v9 == vc9);
}

TEST_CASE("shuffled: restore iteration through a vector in shuffled order "
          "from the index of the last seen element in non shuffled "
		  "container of strings", "[shuffled]") {
  std::list<std::string> ns = {"apple", "banana", "orange", "peach"};
  auto s = shuffled(ns);
  std::list<std::string> v(s.restore(1), std::end(s));
  std::list<std::string> vc = {"banana", "apple"};
  REQUIRE(v == vc);
}

TEST_CASE("shuffled: iterates through a vector in shuffled order with non"
          " standard seed", "[shuffled]") {
  Vec ns = {4, 0, 5, 1, 6, 7, 9, 3, 2, 8};
  auto s = shuffled(ns, 1234);
  Vec v(std::begin(s), std::end(s));
  Vec vc = {4, 2, 9, 8, 6, 7, 5, 3, 1, 0};
  REQUIRE(v == vc);
}

TEST_CASE("shuffled: can modify elements through shuffled", "[shuffled]") {
  std::vector<int> ns(3, 9);
  for (auto&& n : shuffled(ns)) {
    n = -1;
  }
  Vec vc(3, -1);
  REQUIRE(ns == vc);
}

TEST_CASE("shuffled: can iterate over unordered container", "[shuffled]") {
  std::unordered_set<int> ns = {1, 3, 2, 0, 4};
  auto s = shuffled(ns);

  Vec v(std::begin(s), std::end(s));
  Vec vc = {3, 2, 1, 4, 0};
  REQUIRE(v == vc);
}

TEST_CASE("shuffled: empty when iterable is empty", "[shuffled]") {
  Vec ns{};
  auto s = shuffled(ns);
  REQUIRE(std::begin(s) == std::end(s));
}

namespace {
  template <typename T>
  class BasicIterableWithConstDeref {
   private:
    T* data;
    std::size_t size;
    bool was_moved_from_ = false;

   public:
    BasicIterableWithConstDeref(std::initializer_list<T> il)
        : data{new T[il.size()]}, size{il.size()} {
      // would like to use enumerate, can't because it's for unit
      // testing enumerate
      std::size_t i = 0;
      for (auto&& e : il) {
        data[i] = e;
        ++i;
      }
    }

    BasicIterableWithConstDeref& operator=(
        BasicIterableWithConstDeref&&) = delete;
    BasicIterableWithConstDeref& operator=(
        const BasicIterableWithConstDeref&) = delete;
    BasicIterableWithConstDeref(const BasicIterableWithConstDeref&) = delete;

    BasicIterableWithConstDeref(BasicIterableWithConstDeref&& other)
        : data{other.data}, size{other.size} {
      other.data = nullptr;
      other.was_moved_from_ = true;
    }

    bool was_moved_from() const {
      return this->was_moved_from_;
    }

    ~BasicIterableWithConstDeref() {
      delete[] this->data;
    }

    class Iterator
      : public std::iterator<std::input_iterator_tag, T> {
     private:
      T* p;

     public:
      Iterator(T* b) : p{b} {}
      bool operator!=(const Iterator& other) const {
        return this->p != other.p;
      }

      Iterator& operator++() {
        ++this->p;
        return *this;
      }

      T& operator*() {
        return *this->p;
      }

      const T& operator*() const {
        return *this->p;
      }
    };

    Iterator begin() {
      return {this->data};
    }

    Iterator end() {
      return {this->data + this->size};
    }
  };
}

TEST_CASE("shuffled: moves rvalues and binds to lvalues", "[shuffled]") {
  BasicIterableWithConstDeref<int> bi{1, 2};
  shuffled(bi);
  REQUIRE_FALSE(bi.was_moved_from());

  shuffled(std::move(bi));
  REQUIRE(bi.was_moved_from());
}

TEST_CASE("shuffled: doesn't move or copy elements of iterable", "[shuffled]") {
  using itertest::SolidInt;
  constexpr SolidInt arr[] = {{6}, {7}, {8}};
  for (auto &&i : shuffled(arr))
    (void)i;
}

template <typename T>
using ImpT = decltype(shuffled(std::declval<T>()));
TEST_CASE("shuffled: has correct ctor and assign ops", "[shuffled]") {
  REQUIRE(itertest::IsMoveConstructibleOnly<ImpT<std::string&>>::value);
  REQUIRE(itertest::IsMoveConstructibleOnly<ImpT<std::string>>::value);
}

