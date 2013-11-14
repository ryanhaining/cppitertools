#ifndef SLICE_HPP
#define SLICE_HPP

#include "iterator_range.hpp"
#include "wrap_iter.hpp"

#include <iterator>
#include <cassert>

namespace iter {
    template <typename Container, typename DifferenceType>
    auto slice( Container && container,
            DifferenceType begin,
            DifferenceType end,
            DifferenceType step = 1
            ) -> iterator_range<wrap_iter<decltype(std::begin(container))>>
    {
        // Check for an invalid slice. The sign of step must be equal to the
        // sign of (end - begin). Since we don't force DifferenceType to be a
        // primitive, just compare.
        if (!(begin > end && step < 0) && !(begin <= end && step > 0)) {
            // Just in case it gets dereferenced it will be the first element
            // had the range been valid. This handles zero-length slices.
            auto empty = std::next(std::begin(container), begin);
            return iterator_range<wrap_iter<decltype(std::begin(container))>>(
                    make_wrap_iter(empty, step),
                    make_wrap_iter(empty, step));
        }
        // Return the iterator range
        DifferenceType new_end = end - ((end - begin) % step);
        auto begin_iter = std::next(std::begin(container), begin);
        auto end_iter = std::next(std::begin(container), new_end);
        return iterator_range<wrap_iter<decltype(std::begin(container))>>(
                make_wrap_iter(begin_iter,step),
                make_wrap_iter(end_iter,step));

    }
    //only give the end as an arg and assume step  is 1 and begin is 0
    template <typename Container, typename DifferenceType>
    auto slice(
            Container && container,
            DifferenceType end
            ) -> iterator_range<wrap_iter<decltype(std::begin(container))>>
    {
        return slice(std::forward<Container>(container),0,end);
    }

    //Forward declarations of Slice and slice
    template <typename Container>
    class Slice;

    //template <typename T>
    //Slice<std::initializer_list<T>> slice( std::initializer_list<T> &&);

    template <typename Container>
    Slice<Container> slice(Container &&);

    template <typename Container>
    class Slice : public IterBase<Container>{
        private:
            Container & container;

            // The only thing allowed to directly instantiate an Slice is
            // the slice function
            friend Slice slice<Container>(Container &&);
            //template <typename T>
            //friend Slice<std::initializer_list<T>> slice(std::initializer_list<T> &&);

            using typename IterBase<Container>::contained_iter_type;

            using typename IterBase<Container>::contained_iter_ret;

            Slice(Container & container) : container(container) { }
            
        public:
            // Value constructor for use only in the slice function
            Slice () = delete;
            Slice & operator=(const Slice &) = delete;

            Slice(const Slice &) = default;


            //  Holds an iterator of the contained type and a size_t for the
            //  index.  Each call to ++ increments both of these data members.
            //  Each dereference returns an IterYield.
            class Iterator {
                private:
                    contained_iter_type sub_iter;
                public:
                    Iterator (contained_iter_type si) :
                        sub_iter(si)
                    { }

                    IterYield operator*() const {
                        return IterYield(this->index, *this->sub_iter);
                    }

                    Iterator & operator++() { 
                        ++this->sub_iter;
                        return *this;
                    }

                    bool operator!=(const Iterator & other) const {
                        return this->sub_iter != other.sub_iter;
                    }
            };

            Iterator begin() const {
                return Iterator(std::begin(this->container));
            }

            Iterator end() const {
                return Iterator(std::end(this->container));
            }

    };
    // Helper function to instantiate an Slice
    template <typename Container>
    Slice<Container> slice(Container && container) {
        return Slice<Container>(std::forward<Container>(container));
    }

    template <typename T>
    Slice<std::initializer_list<T>> slice(std::initializer_list<T> && il)
    {
        return Slice<std::initializer_list<T>>(il);
    }

}

#endif //SLICE_HPP
