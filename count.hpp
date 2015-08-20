#ifndef ITER_COUNT_H_
#define ITER_COUNT_H_

#include "range.hpp"

#include <limits>

namespace iter {

  template <typename T>
  constexpr auto count(T start, T step) noexcept
      -> decltype(range(start, start, start)) {
    // if step is < 0, stop is numeric min, otherwise numeric max
    return range(start, step < T(0) ? std::numeric_limits<T>::min()
                                    : std::numeric_limits<T>::max(),
        step);
  }

  template <typename T = long>
  constexpr auto count(T start = T(0)) noexcept
      -> decltype(range(start, start)) {
    return count(start, T(1));
  }
}

#endif
