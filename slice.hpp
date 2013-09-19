#ifndef SLICE_HPP
#define SLICE_HPP

#include <iterator>
#include <iterator_range.hpp>
#include <wrap_iter.hpp>
#include <assert.h>
/* I plan to have slice support steps like slice(vec,0,10,2)
 * and negative ranges like slice(vec,10,4)
 * could also write an overload so you can default to start=0
 */
namespace iter {
    template <typename Container>
        auto slice(
                Container & container,
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
            else assert(false); //not a valid range
        }
}

#endif //SLICE_HPP
