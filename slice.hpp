#ifndef SLICE_HPP
#define SLICE_HPP

#include "iterator_range.hpp"
#include "wrap_iter.hpp"

#include <iterator>
#include <assert.h>

namespace iter {
    template <typename Container>
        auto slice(
                Container && container,
                typename std::iterator_traits<decltype(container.begin())>::difference_type begin,
                typename std::iterator_traits<decltype(container.begin())>::difference_type end,
                typename std::iterator_traits<decltype(container.begin())>::difference_type step = 1
                ) -> iterator_range<wrap_iter<decltype(container.begin())>>
        {
            //it seems like you can handle negative and positive ranges the same
            if (begin > end && step < 0) {
                typename std::iterator_traits<decltype(container.begin())>::difference_type new_end = end - ((end - begin) % step);
                return iterator_range<wrap_iter<decltype(container.begin())>>(
                        make_wrap_iter(container.begin()+begin,step),
                        make_wrap_iter(container.begin()+new_end,step));
            }
            else if (begin <= end && step > 0) {
                typename std::iterator_traits<decltype(container.begin())>::difference_type new_end = end - ((end - begin) % step);
                return iterator_range<wrap_iter<decltype(container.begin())>>(
                        make_wrap_iter(container.begin()+begin,step),
                        make_wrap_iter(container.begin()+new_end,step));
            }
            else {//return an empty range for invalid slice
                return iterator_range<wrap_iter<decltype(container.begin())>>(
                        make_wrap_iter(container.begin()+begin,step),
                        make_wrap_iter(container.begin()+begin,step));
            }

        }
    //only give the end as an arg and assume step  is 1 and begin is 0
    template <typename Container>
        auto slice(
                Container && container,
                typename std::iterator_traits<decltype(container.begin())>::difference_type end
                ) -> iterator_range<wrap_iter<decltype(container.begin())>>
        {
            return slice(container,0,end);
        }

}

#endif //SLICE_HPP
