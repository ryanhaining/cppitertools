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
            //no real error checking is done either, like for invalid ranges
            //no backwards range yet

            if (begin > end || step < 1) {
                assert(false);
            }
            else {
                typename std::iterator_traits<decltype(container.begin())>::difference_type new_end = end - ((end - begin) % step);
                return iterator_range<wrap_iter<decltype(container.begin())>>(
                        make_wrap_iter(container.begin()+begin,step),
                        make_wrap_iter(container.begin()+new_end,step));
            }
        }
}

#endif //SLICE_HPP
