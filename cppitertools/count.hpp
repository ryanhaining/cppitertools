#ifndef ITER_COUNT_H_
#define ITER_COUNT_H_

#include "range.hpp"

#include <limits>

namespace iter {
  template <typename T>
  constexpr auto count(T start, T step) noexcept {
    T stop = step < T(0) ? std::numeric_limits<T>::lowest()
                         : std::numeric_limits<T>::max();
    return range(start, stop, step);
  }

  template <typename T = long>
  constexpr auto count(T start = T(0)) noexcept {
    return count(start, T(1));
  }
}

#endif
