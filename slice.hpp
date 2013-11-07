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

}

#endif //SLICE_HPP
