#ifndef SLICE_HPP
#define SLICE_HPP

#include <iterator>
#include "iterator_range.hpp"

namespace iter {
    template <typename Container>
        auto slice(
                Container & container,
                typename std::iterator_traits<decltype(container.begin())>::difference_type begin,
                typename std::iterator_traits<decltype(container.begin())>::difference_type end
                ) -> iterator_range<decltype(container.begin())>
        {
            return iterator_range<decltype(container.begin())>(
                    container.begin()+begin,
                    container.begin()+end);
        }
}

#endif //SLICE_HPP
