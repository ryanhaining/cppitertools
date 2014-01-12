#ifndef ITERBASE__HPP__
#define ITERBASE__HPP__


// This file consists of utilities used for the generic nature of the
// iterable wrapper classes.  As such, the contents of this file should be
// considered UNDOCUMENTED and is subject to change without warning.  This
// also applies to the name of the file.  No user code should include
// this file directly.

#include <utility>
#include <iterator>

namespace iter {
    // iterator_type<C> is the type of C's iterator
    template <typename Container>
    using iterator_type =
        decltype(std::begin(std::declval<Container&>()));

    // iterator_deref<C> is the type obtained by dereferencing an iterator
    // to an object of type C
    template <typename Container>
    using iterator_deref = 
        decltype(*std::declval<iterator_type<Container>&>());
}

#endif // #ifndef ITERBASE__HPP__
