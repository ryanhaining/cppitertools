#include "range.hpp"

#include <algorithm>
#include <iterator>
#include <vector>

#include "catch.hpp"
#include "helpers.hpp"

using Vec = const std::vector<int>;
using iter::range;

TEST_CASE("range: .start(), .stop(), and .step()", "[range]") {
  SECTION("one arg") {
    auto r = range(3);
    REQUIRE(r.start() == 0);
    REQUIRE(r.stop() == 3);
    REQUIRE(r.step() == 1);

    // make sure iterators aren't changing the value
    auto it = r.begin();
    ++it;

    REQUIRE(r.start() == 0);
    REQUIRE(r.stop() == 3);
    REQUIRE(r.step() == 1);
  }

  SECTION("two args") {
    auto r = range(2, 10);
    REQUIRE(r.start() == 2);
    REQUIRE(r.stop() == 10);
    REQUIRE(r.step() == 1);
  }

  SECTION("three args") {
    auto r = range(-6, 20, 3);
    REQUIRE(r.start() == -6);
    REQUIRE(r.stop() == 20);
    REQUIRE(r.step() == 3);
  }

  SECTION("one arg (double)") {
    auto r = range(3.5);
    REQUIRE(r.start() == 0);
    REQUIRE(r.stop() == Approx(3.5));
    REQUIRE(r.step() == Approx(1.0));
  }

  SECTION("two args (double)") {
    auto r = range(20.1, 31.7);
    REQUIRE(r.start() == Approx(20.1));
    REQUIRE(r.stop() == Approx(31.7));
    REQUIRE(r.step() == Approx(1.0));
  }

  SECTION("three args (double)") {
    auto r = range(-6.3, 5.7, 0.1);
    REQUIRE(r.start() == Approx(-6.3));
    REQUIRE(r.stop() == Approx(5.7));
    REQUIRE(r.step() == Approx(0.1));
  }
}

TEST_CASE("range: works with only stop", "[range]") {
  auto r = range(5);
  Vec v(std::begin(r), std::end(r));
  Vec vc{0, 1, 2, 3, 4};

  REQUIRE(v == vc);
}

TEST_CASE("range: works with start and stop", "[range]") {
  auto r = range(1, 5);
  Vec v(std::begin(r), std::end(r));
  Vec vc{1, 2, 3, 4};

  REQUIRE(v == vc);
}

TEST_CASE("range: works with positive step > 1", "[range]") {
  auto r = range(1, 10, 3);
  Vec v(std::begin(r), std::end(r));
  Vec vc{1, 4, 7};

  REQUIRE(v == vc);
}

TEST_CASE("range(0) is empty", "[range]") {
  auto r = iter::range(0);
  REQUIRE(std::begin(r) == std::end(r));
}

TEST_CASE("range: postfix++", "[range]") {
  auto r = iter::range(3);
  auto it = std::begin(r);
  it++;
  REQUIRE(*it == 1);
}

TEST_CASE("start > stop produces empty range", "[range]") {
  auto r = range(5, 0);
  Vec v(std::begin(r), std::end(r));
  REQUIRE(v.empty());
}

TEST_CASE("start < stop and step < 0 produces empty range", "[range]") {
  auto r = range(0, 5, -1);
  Vec v(std::begin(r), std::end(r));
  REQUIRE(v.empty());
}

TEST_CASE("range: with only a negative stop is empty", "[range]") {
  auto r = range(-3);
  Vec v(std::begin(r), std::end(r));

  REQUIRE(v.empty());
}

TEST_CASE("range: works with negative step", "[range]") {
  auto r = range(5, -5, -3);
  Vec v(std::begin(r), std::end(r));
  Vec vc{5, 2, -1, -4};

  REQUIRE(v == vc);
}

TEST_CASE("range: stops short when step doesn't divide stop-start", "[range]") {
  auto r = range(0, 5, 2);
  Vec v(std::begin(r), std::end(r));
  Vec vc{0, 2, 4};

  REQUIRE(v == vc);
}

TEST_CASE("range: stops short when step > stop-start", "[range]") {
  auto r = range(0, 10, 20);
  Vec v(std::begin(r), std::end(r));
  REQUIRE(v.size() == 1);
}

TEST_CASE("range: step size of 0 gives an empty range", "[range]") {
  auto r = range(0, 10, 0);
  REQUIRE(std::begin(r) == std::end(r));
  auto r2 = range(0, -10, 0);
  REQUIRE(std::begin(r2) == std::end(r2));
}

TEST_CASE("range: can create constexpr ranges", "[range]") {
  constexpr auto r = range(10);
  (void)r;
  constexpr auto r2 = range(4, 10);
  (void)r2;
  constexpr auto r3 = range(4, 10, 2);
  (void)r3;

  constexpr auto it = r2.begin();  // std::begin isn't constexpr
  constexpr auto i = *it;
  static_assert(i == 4, "range's begin has the wrong value");

  constexpr auto rf = range(10.0);
  constexpr auto itf = rf.begin();
  constexpr auto f = *itf;
  static_assert(f == 0.0, "range's begin has tho wrong value (float)");
}

TEST_CASE("range: const iterators compare to non-const iterators", "[range]") {
  auto r = range(0);
  const auto& cr = r;
  (void)(std::begin(r) == std::end(cr));
}

TEST_CASE("range: works with a variable start, stop, and step", "[range]") {
  constexpr int a = 10;
  constexpr int b = 100;
  constexpr int c = 50;
  SECTION("Going up works") {
    auto r = range(a, a + 2);
    Vec v(std::begin(r), std::end(r));
    Vec vc{a, a + 1};
    REQUIRE(v == vc);
  }

  SECTION("Going down works") {
    auto r = range(a + 2, a, -1);
    Vec v(std::begin(r), std::end(r));
    Vec vc{a + 2, a + 1};
    REQUIRE(v == vc);
  }

  SECTION("Going down with -2 stop works") {
    auto r = range(a + 4, a, -2);
    Vec v(std::begin(r), std::end(r));
    Vec vc{a + 4, a + 2};
    REQUIRE(v == vc);
  }

  SECTION("Using three variable") {
    auto r = range(a, b, c);
    Vec v(std::begin(r), std::end(r));
    REQUIRE(std::find(std::begin(v), std::end(v), a) != std::end(v));
    REQUIRE(std::find(std::begin(v), std::end(v), b) == std::end(v));
    REQUIRE(v.size() == 2);
  }

  SECTION("Using three with a unary negate on step") {
    auto r = range(b, a, -c);
    Vec v(std::begin(r), std::end(r));

    REQUIRE(std::find(std::begin(v), std::end(v), b) != std::end(v));
    REQUIRE(std::find(std::begin(v), std::end(v), a) == std::end(v));
    REQUIRE(v.size() == 2);
  }

  SECTION("Using all three negated") {
    auto r = range(-a, -b, -c);
    Vec v(std::begin(r), std::end(r));

    REQUIRE(std::find(std::begin(v), std::end(v), -a) != std::end(v));
    REQUIRE(std::find(std::begin(v), std::end(v), -b) == std::end(v));
    REQUIRE(v.size() == 2);
  }
}

TEST_CASE("range: forward iterator checks", "[range]") {
  auto r = range(10);
  REQUIRE(std::end(r) == std::end(r));
  auto it1 = std::begin(r);
  auto it2 = std::begin(r);
  REQUIRE_FALSE(it1 != it2);
  REQUIRE(it1 == it2);
  ++it1;
  REQUIRE(it1 != it2);
  ++it2;
  REQUIRE(it1 == it2);
  auto it3 = it1++;
  REQUIRE(it3 == it2);
  auto it4 = ++it3;
  REQUIRE(it4 == it3);

  auto it5 = std::begin(r);
  const auto& v = *it5;
  ++it5;
  REQUIRE(v != *it5);
}

TEST_CASE("range: forward iterator with double, checks", "[range]") {
  auto r = range(10.0);
  REQUIRE(std::end(r) == std::end(r));
  auto it1 = std::begin(r);
  auto it2 = std::begin(r);
  REQUIRE_FALSE(it1 != it2);
  REQUIRE(it1 == it2);
  ++it1;
  REQUIRE(it1 != it2);
  ++it2;
  REQUIRE(it1 == it2);
  auto it3 = it1++;
  REQUIRE(it3 == it2);
  auto it4 = ++it3;
  REQUIRE(it4 == it3);
}

using FVec = const std::vector<double>;

TEST_CASE("range: using doubles", "[range]") {
  auto r = range(5.0);
  FVec fv(std::begin(r), std::end(r));
  FVec fvc = {0.0, 1.0, 2.0, 3.0, 4.0};
  REQUIRE(fv == fvc);
}

TEST_CASE("range: using doubles with start and stop", "[range]") {
  auto r = range(5.0, 10.0);
  FVec fv(std::begin(r), std::end(r));
  FVec fvc = {5.0, 6.0, 7.0, 8.0, 9.0};
  REQUIRE(fv == fvc);
}

TEST_CASE("range: using doubles with start, stop and step", "[range]") {
  auto r = range(1.0, 4.0, 0.5);
  FVec fv(std::begin(r), std::end(r));
  FVec fvc = {1.0, 1.5, 2.0, 2.5, 3.0, 3.5};
  REQUIRE(fv == fvc);
}

TEST_CASE("range: using doubles with negative", "[range]") {
  auto r = range(0.5, -2.0, -0.5);
  FVec fv(std::begin(r), std::end(r));
  FVec fvc = {0.5, 0.0, -0.5, -1.0, -1.5};
  REQUIRE(fv == fvc);
}

TEST_CASE("range: using doubles with uneven step", "[range]") {
  auto r = range(0.0, 1.75, 0.5);
  FVec fv(std::begin(r), std::end(r));
  FVec fvc = {0.0, 0.5, 1.0, 1.5};
  REQUIRE(fv == fvc);
}

TEST_CASE("range: using doubles detects empty ranges", "[range]") {
  auto r1 = range(0.0, -1.0);
  REQUIRE(std::begin(r1) == std::end(r1));

  auto r2 = range(0.0, 1.0, -1.0);
  REQUIRE(std::begin(r2) == std::end(r2));
}

TEST_CASE("range: iterator meets forward iterator requirements", "[range]") {
  auto r = range(5);
  auto r2 = range(5.0);
  REQUIRE(itertest::IsForwardIterator<decltype(std::begin(r))>::value);
  REQUIRE(itertest::IsForwardIterator<decltype(std::begin(r2))>::value);
}

TEST_CASE("range: operator[] simple tests", "[range]") {
  SECTION("range(start)") {
    auto r = range(4);
    REQUIRE(r[0] == 0);
    REQUIRE(r[1] == 1);
    REQUIRE(r[2] == 2);
    REQUIRE(r[3] == 3);
  }
  SECTION("range(start, stop)") {
    auto r = range(10, 14);
    REQUIRE(r[0] == 10);
    REQUIRE(r[1] == 11);
    REQUIRE(r[2] == 12);
    REQUIRE(r[3] == 13);
  }
  SECTION("range(start, stop, step)") {
    auto r = range(20, 30, 3);
    REQUIRE(r[0] == 20);
    REQUIRE(r[1] == 23);
    REQUIRE(r[2] == 26);
    REQUIRE(r[3] == 29);
  }
  SECTION("range(start, stop, step) with double") {
    auto r = range(50.0, 50.99, 0.2);
    REQUIRE(r[0] == Approx(50.0));
    REQUIRE(r[1] == Approx(50.2));
    REQUIRE(r[2] == Approx(50.4));
    REQUIRE(r[3] == Approx(50.6));
    REQUIRE(r[4] == Approx(50.8));
  }
}

TEMPLATE_TEST_CASE("range: .size() with signed integrals", "[range]",
    signed char, short, int, long, long long) {
  constexpr TestType N = 5;
  constexpr TestType INC = 1;
  for (TestType start = -N; start < N; start += INC) {
    for (TestType stop = -N; stop < N; stop += INC) {
      for (TestType step = -N; step < N; step += INC) {
        if (step == 0) {
          continue;
        }
        auto r = range(start, stop, step);
        REQUIRE(r.size()
                == static_cast<std::size_t>(
                       std::distance(std::begin(r), std::end(r))));
      }
    }
  }
}

TEMPLATE_TEST_CASE("range: .size() with unsigned integrals", "[range]",
    unsigned char, unsigned short, unsigned int, unsigned long,
    unsigned long long) {
  constexpr TestType N = 5;
  constexpr TestType INC = 1;
  for (TestType start = 0; start < N; start += INC) {
    for (TestType stop = 0; stop < N; stop += INC) {
      for (TestType step = 0; step < N; step += INC) {
        if (step == 0) {
          continue;
        }
        auto r = range(start, stop, step);
        REQUIRE(r.size()
                == static_cast<std::size_t>(
                       std::distance(std::begin(r), std::end(r))));
      }
    }
  }
}
