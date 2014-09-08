#ifndef ITERBASE_HPP_
#define ITERBASE_HPP_


// This file consists of utilities used for the generic nature of the
// iterable wrapper classes.  As such, the contents of this file should be
// considered UNDOCUMENTED and is subject to change without warning.  This
// also applies to the name of the file.  No user code should include
// this file directly.

#include <utility>
#include <iterator>
#include <cstddef>

namespace iter {
    // because std::advance assumes a lot and is actually smart, I need a dumb
    // version that will work with most things
    template <typename InputIt, typename Distance>
    void dumb_advance(InputIt& iter, Distance distance) {
        for (Distance i(0); i < distance; ++i) {
            ++iter;
        }
    }

    // iter will not be incremented past end
    template <typename InputIt, typename Distance>
    void dumb_advance(InputIt& iter, const InputIt& end, Distance distance=1) {
        for (Distance i(0); i < distance && iter != end; ++i) {
            ++iter;
        }
    }

    template <typename ForwardIt, typename Distance>
    ForwardIt dumb_next(ForwardIt it, Distance distance) {
        dumb_advance(it, distance);
        return it;
    }

    template <typename ForwardIt, typename Distance>
    ForwardIt dumb_next(
            ForwardIt it, const ForwardIt& end, Distance distance=1) {
        dumb_advance(it, end, distance);
        return it;
    }


    // iterator_type<C> is the type of C's iterator
    template <typename Container>
    using iterator_type =
        decltype(std::begin(std::declval<Container&>()));

    // iterator_deref<C> is the type obtained by dereferencing an iterator
    // to an object of type C
    template <typename Container>
    using iterator_deref = 
        decltype(*std::declval<iterator_type<Container>&>());

    // iterator_type<C> is the type of C's iterator
    template <typename Container>
    using reverse_iterator_type =
        decltype(std::declval<Container&>().rbegin());

    // iterator_deref<C> is the type obtained by dereferencing an iterator
    // to an object of type C
    template <typename Container>
    using reverse_iterator_deref = 
        decltype(*std::declval<reverse_iterator_type<Container>&>());
}

#endif // #ifndef ITERBASE_HPP_
