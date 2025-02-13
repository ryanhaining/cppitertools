#include <cppitertools/unique_justseen.hpp>

#include "helpers.hpp"

#include <iterator>
#include <string>
#include <vector>

#include "catch.hpp"

using iter::unique_justseen;

using Vec = std::vector<int>;

TEST_CASE("unique justseen: adjacent repeating values", "[unique_justseen]") {
  Vec ns = {1, 1, 1, 2, 2, 3, 4, 4, 5, 6, 7, 8, 8, 8, 8, 9, 9};
  auto ue = unique_justseen(ns);
  Vec v(std::begin(ue), std::end(ue));
  Vec vc = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  REQUIRE(v == vc);
}

TEST_CASE("unique justseen: const iteration", "[unique_justseen][const]") {
  Vec ns = {1, 1, 1, 2, 2, 3, 4, 4, 5, 6, 7, 8, 8, 8, 8, 9, 9};
  const auto uj = unique_justseen(ns);
  Vec v(std::begin(uj), std::end(uj));
  Vec vc = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  REQUIRE(v == vc);
}

TEST_CASE(
    "unique justseen: const iterator can be compared to non-const iterators",
    "[unique_justseen][const]") {
  auto uj = unique_justseen(Vec{});
  const auto& cuj = uj;
  (void)(std::begin(uj) == std::begin(cuj));
}

TEST_CASE("unique justseen: some repeating values", "[unique_justseen]") {
  Vec ns = {1, 2, 2, 3, 4, 4, 5, 6, 6};
  std::vector<int> v;
  SECTION("Normal call") {
    auto ue = unique_justseen(ns);
    v.assign(std::begin(ue), std::end(ue));
  }
  SECTION("Pipe") {
    auto ue = ns | unique_justseen;
    v.assign(std::begin(ue), std::end(ue));
  }
  Vec vc = {1, 2, 3, 4, 5, 6};
  REQUIRE(v == vc);
}

TEST_CASE("unique justseen: Works with different begin and end types",
    "[unique_justseen]") {
  CharRange cr{'d'};
  using VecC = std::vector<char>;
  auto uj = unique_justseen(cr);
  VecC v(uj.begin(), uj.end());
  VecC vc{'a', 'b', 'c'};
  REQUIRE(v == vc);
}

TEST_CASE("unique justseen: doesn't omit non-adjacent duplicates",
    "[unique_justseen]") {
  Vec ns = {1, 2, 3, 2, 1, 2, 3, 2, 1};
  auto ue = unique_justseen(ns);
  Vec v(std::begin(ue), std::end(ue));
  REQUIRE(v == ns);
}

TEST_CASE("unique justseen: moves and binds correctly", "[unique_justseen]") {
  itertest::BasicIterable<int> bi{1, 2};
  unique_justseen(bi);
  REQUIRE_FALSE(bi.was_moved_from());

  unique_justseen(std::move(bi));
  REQUIRE(bi.was_moved_from());
}

TEST_CASE("unique_justseen: iterator meets requirements", "[unique_justseen]") {
  std::string s{};
  auto c = unique_justseen(s);
  REQUIRE(itertest::IsIterator<decltype(std::begin(c))>::value);
}

template <typename T>
using ImpT = decltype(unique_justseen(std::declval<T>()));
TEST_CASE(
    "unique_justseen: has correct ctor and assign ops", "[unique_justseen]") {
  REQUIRE(itertest::IsMoveConstructibleOnly<ImpT<std::string&>>::value);
  REQUIRE(itertest::IsMoveConstructibleOnly<ImpT<std::string>>::value);
}

struct IntWrapper {
  int n;
};

struct IntWrapperKey {
  int operator()(const IntWrapper& iw) const {
    return iw.n;
  }
};

struct MoveOnlyIntWrapperKey {
  MoveOnlyIntWrapperKey(const MoveOnlyIntWrapperKey&) = delete;
  MoveOnlyIntWrapperKey& operator=(const MoveOnlyIntWrapperKey&) = delete;

  MoveOnlyIntWrapperKey(MoveOnlyIntWrapperKey&&) = default;
  MoveOnlyIntWrapperKey& operator=(MoveOnlyIntWrapperKey&&) = default;
  int operator()(const IntWrapper& iw) const {
    return iw.n;
  }
};

TEST_CASE("unique_justseen: works with key function", "[unique_justseen]") {
  std::vector<IntWrapper> iwv = {
      {2}, {3}, {4}, {2}, {10}, {2}, {2}, {12}, {10}};
  Vec vc{2, 3, 4, 2, 10, 2, 12, 10};

  std::vector<int> v;
  SECTION("with callable") {
    SECTION("Normal call") {
      for (auto&& iw : unique_justseen(iwv, IntWrapperKey{})) {
        v.push_back(iw.n);
      }
    }
    SECTION("Pipe") {
      for (auto&& iw : iwv | unique_justseen(IntWrapperKey{})) {
        v.push_back(iw.n);
      }
    }
  }
  SECTION("with move-only callable") {
    SECTION("Normal call") {
      for (auto&& iw : unique_justseen(iwv, MoveOnlyIntWrapperKey{})) {
        v.push_back(iw.n);
      }
    }
    SECTION("Pipe") {
      for (auto&& iw : iwv | unique_justseen(MoveOnlyIntWrapperKey{})) {
        v.push_back(iw.n);
      }
    }
  }

  REQUIRE(v == vc);
}
