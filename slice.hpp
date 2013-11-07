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
        //it seems like you can handle negative and positive ranges the same
        //kept both checks to make checking for invalid slice more readable
        if (begin > end && step < 0) {
            DifferenceType new_end = end - ((end - begin) % step);
            auto begin_iter = std::begin(container);
            std::advance(begin_iter,begin);
            auto end_iter = std::begin(container);
            std::advance(end_iter,new_end);
            return iterator_range<wrap_iter<decltype(std::begin(container))>>(
                    make_wrap_iter(begin_iter,step),
                    make_wrap_iter(end_iter,step));
        }
        else if (begin <= end && step > 0) {
            DifferenceType new_end = end - ((end - begin) % step);
            auto begin_iter = std::begin(container);
            std::advance(begin_iter,begin);
            auto end_iter = std::begin(container);
            std::advance(end_iter,new_end);
            return iterator_range<wrap_iter<decltype(std::begin(container))>>(
                    make_wrap_iter(begin_iter,step),
                    make_wrap_iter(end_iter,step));
        }
        else {//return an empty range for invalid slice
            auto empty = std::begin(container);
            std::advance(empty,begin);
            return iterator_range<wrap_iter<decltype(std::begin(container))>>(
                    make_wrap_iter(empty,step),
                    make_wrap_iter(empty,step));
        }

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
