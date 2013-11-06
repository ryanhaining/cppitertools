#ifndef ITERBASE__HPP__
#define ITERBASE__HPP__

#include <utility>
#include <iterator>

namespace iter {
    template <typename Iterable>
    class IterBase{
        protected:
            // Type of the Container::Iterator, but since the name of that 
            // iterator can be anything, we have to grab it with this
            using contained_iter_type =
                decltype(std::begin(std::declval<Iterable&>()));

            // The type returned when dereferencing the Container::Iterator
            using contained_iter_ret =
                decltype(std::declval<contained_iter_type&>().operator*());
            
            IterBase() = default;
            IterBase(const IterBase &) = default;
            IterBase & operator=(const IterBase &) = delete;
    };
}

#endif // #ifndef ITERBASE__HPP__
