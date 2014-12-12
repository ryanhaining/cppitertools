#include <dropwhile.hpp>

#include "helpers.hpp"

#include <vector>
#include <string>
#include <iterator>

#include "catch.hpp"

using iter::dropwhile;

using Vec = const std::vector<int>;

TEST_CASE("dropwhile: skips initial elements", "[dropwhile]") {
    Vec ns{1,2,3,4,5,6,7,8};
    auto d = dropwhile([](int i){return i < 5; }, ns);
    Vec v(std::begin(d), std::end(d));
    Vec vc = {5,6,7,8};
    REQUIRE( v == vc );
}

TEST_CASE("dropwhile: doesn't skip anything if it shouldn't", "[dropwhile]") {
    Vec ns {3,4,5,6};
    auto d = dropwhile([](int i){return i < 3; }, ns);
    Vec v(std::begin(d), std::end(d));
    Vec vc = {3,4,5,6};
    REQUIRE( v == vc );
}

TEST_CASE("dropwhile: skips all elements when all are true under predicate",
        "[dropwhile]") {
    Vec ns {3,4,5,6};
    auto d = dropwhile([](int i){return i != 0; }, ns);
    REQUIRE( std::begin(d) == std::end(d) );
}

TEST_CASE("dropwhile: empty case is empty", "[dropwhile]") {
    Vec ns{};
    auto d = dropwhile([](int i){return i != 0; }, ns);
    REQUIRE( std::begin(d) == std::end(d) );
}

TEST_CASE("dropwhile: only drops from beginning", "[dropwhile]") {
    Vec ns {1,2,3,4,5,6,5,4,3,2,1};
    auto d = dropwhile([](int i){return i < 5; }, ns);
    Vec v(std::begin(d), std::end(d));
    Vec vc = {5,6,5,4,3,2,1};
    REQUIRE( v == vc );
}
