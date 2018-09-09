#include <sorted.hpp>

#include <array>
#include <set>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "catch.hpp"
#include "helpers.hpp"

using iter::sorted;

using Vec = const std::vector<int>;

TEST_CASE("sorted: iterates through a vector in sorted order", "[sorted]") {
  Vec ns = {4, 0, 5, 1, 6, 7, 9, 3, 2, 8};
  std::vector<int> v;
  SECTION("Normal Call") {
    auto s = sorted(ns);
    v.assign(std::begin(s), std::end(s));
  }
  SECTION("Pipe") {
    auto s = ns | sorted;
    v.assign(std::begin(s), std::end(s));
  }
  Vec vc = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  REQUIRE(v == vc);
}

TEST_CASE("sorted: handles pointer to member function", "[sorted]") {
  using itertest::Point;
  const std::vector<Point> ps = {{5, 0}, {3, 0}, {10, 0}, {6, 0}};
  auto s = sorted(ps, &Point::left_of);
  const std::vector<Point> v(std::begin(s), std::end(s));
  const std::vector<Point> vc = {{3, 0}, {5, 0}, {6, 0}, {10, 0}};
  REQUIRE(v == vc);
}

TEST_CASE("sorted: const iteration", "[sorted][const]") {
  Vec ns = {4, 0, 5, 1, 6, 7, 9, 3, 2, 8};
  const auto s = sorted(ns);
  Vec v(std::begin(s), std::end(s));
  Vec vc = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  REQUIRE(v == vc);
}

//FIXME: This test currently fails (STL assertion fails on MSVC with debug library, simple test failure on gcc).  The problem is 'sorted' will sort twice, once for non-const and once for const container; the resulting iterators are thus not on the same container (violating domain of == as specified in C++17 [forward.iterators]¶2).  Remove [!hide] tag when fixed.
TEST_CASE("sorted: const iterators can be compared to non-const iterators",
    "[sorted][const][!hide]") {
  auto s = sorted(Vec{1});
  const auto& cs = s;
  REQUIRE(std::begin(s) == std::begin(cs));
}

TEST_CASE("sorted: can modify elements through sorted", "[sorted]") {
  std::vector<int> ns(3, 9);
  for (auto&& n : sorted(ns)) {
    n = -1;
  }
  Vec vc(3, -1);
  REQUIRE(ns == vc);
}

char inc_vowels(char c) {
  return c == 'a' || c == 'e' ? c + 10 : c;
}

TEST_CASE("sorted: Works with different begin and end types", "[sorted]") {
  using VecC = std::vector<char>;
  CharRange cr{'g'};
  auto s =
      sorted(cr, [](char x, char y) { return inc_vowels(x) < inc_vowels(y); });
  VecC v(s.begin(), s.end());
  VecC vc{'b', 'c', 'd', 'f', 'a', 'e'};
  REQUIRE(v == vc);
}

TEST_CASE("sorted: can iterate over unordered container", "[sorted]") {
  std::unordered_set<int> ns = {1, 3, 2, 0, 4};
  auto s = sorted(ns);

  Vec v(std::begin(s), std::end(s));
  Vec vc = {0, 1, 2, 3, 4};
  REQUIRE(v == vc);
}

TEST_CASE("sorted: empty when iterable is empty", "[sorted]") {
  Vec ns{};
  auto s = sorted(ns);
  REQUIRE(std::begin(s) == std::end(s));
}

namespace {
  bool int_greater_than(int lhs, int rhs) {
    return lhs > rhs;
  }

  struct IntGreaterThan {
    bool operator()(int lhs, int rhs) const {
      return lhs > rhs;
    }
  };
}

TEST_CASE("sorted: works with different callable types", "[sorted]") {
  Vec ns = {4, 1, 3, 2, 0};
  std::vector<int> v;
  SECTION("with function pointer") {
    auto s = sorted(ns, int_greater_than);
    v.assign(std::begin(s), std::end(s));
  }

  SECTION("with callable object") {
    SECTION("Normal call") {
      auto s = sorted(ns, IntGreaterThan{});
      v.assign(std::begin(s), std::end(s));
    }
    SECTION("Pipe") {
      auto s = ns | sorted(IntGreaterThan{});
      v.assign(std::begin(s), std::end(s));
    }
  }

  SECTION("with lambda") {
    auto s = sorted(ns, [](int lhs, int rhs) { return lhs > rhs; });
    v.assign(std::begin(s), std::end(s));
  }

  Vec vc = {4, 3, 2, 1, 0};
  REQUIRE(v == vc);
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

    class Iterator {
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

TEST_CASE("sorted: moves rvalues and binds to lvalues", "[sorted]") {
  BasicIterableWithConstDeref<int> bi{1, 2};
  sorted(bi);
  REQUIRE_FALSE(bi.was_moved_from());

  sorted(std::move(bi));
  REQUIRE(bi.was_moved_from());
}

TEST_CASE("sorted: doesn't move or copy elements of iterable", "[sorted]") {
  using itertest::SolidInt;
  constexpr SolidInt arr[] = {{6}, {7}, {8}};
  for (auto &&i : sorted(arr, [](const SolidInt&lhs, const SolidInt&rhs) {
         return lhs.getint() < rhs.getint();
       })) {
    (void)i;
  }
}

template <typename T>
using ImpT = decltype(sorted(std::declval<T>()));
TEST_CASE("sorted: has correct ctor and assign ops", "[sorted]") {
  REQUIRE(itertest::IsMoveConstructibleOnly<ImpT<std::string&>>::value);
  REQUIRE(itertest::IsMoveConstructibleOnly<ImpT<std::string>>::value);
}

TEST_CASE("sorted: iterator meets requirements", "[sorted]") {
  Vec v;
  auto r = sorted(v);
  REQUIRE(itertest::IsIterator<decltype(std::begin(r))>::value);
}
