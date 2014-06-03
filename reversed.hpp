#ifndef REVERSE_HPP
#define REVERSE_HPP

#include "iterator_range.hpp"

namespace iter {
    template <typename Container>
       auto reverse(Container && container) -> iterator_range<decltype(container.rbegin())>
       {
           return 
               iterator_range<decltype(container.rbegin())>(container.rbegin(),container.rend());
       }
}

#endif //REVERSE_HPP
