// mixing different itertools, there is nothing called iter::mixed()


#include "imap.hpp"
#include "filter.hpp"

#include "catch.hpp"

#include <iostream>
#include <vector>

using iter::filter;
using iter::imap;

class MyUnMovable {
    int val;
public:
    constexpr MyUnMovable(int val)
        : val{val}
    { }

    MyUnMovable(const MyUnMovable&) = delete;
    MyUnMovable& operator=(const MyUnMovable&) = delete;

    MyUnMovable(MyUnMovable&& other)
        : val{other.val}
    { }

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

TEST_CASE("imap and filter where imap Functor modifies its sequence",
    "[imap][filter]") {

    // source data
    std::array<MyUnMovable, 3> arr = {{{41}, {42}, {43}}};

    // some transformations
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

    auto transformed1 = imap(inc_ten, arr);
    auto filtered = filter([](const MyUnMovable& el) {
        return 52 != el.get_val();
    }, transformed1);
    auto transformed2 = imap(dec_ten, filtered);

    std::vector<int> v;
    for (auto&& el : transformed2) {
        // I would use imap again instead of the loop if this wasn't an imap
        // test
        v.push_back(el.get_val());
    }

    std::vector<int> vc = {41, 43};

    REQUIRE( v == vc);

    constexpr std::array<MyUnMovable, 3> arrc = {{{41}, {52}, {43}}};
    REQUIRE( arr == arrc );
}
