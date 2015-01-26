#ifndef ITER_COUNT_H_
#define ITER_COUNT_H_

#include "range.hpp"

#include <limits>

namespace iter {

    using DefaultRangeType = long;

    auto count() -> decltype(range(DefaultRangeType(0), DefaultRangeType(0))) {
        return range(DefaultRangeType(0),
                std::numeric_limits<DefaultRangeType>::max());
    }

    template <typename T>
    auto count(T start, T step) -> decltype(range(start, start, start)) {
        // if step is < 0, set the stop to numeric min, otherwise numeric max
        T stop = step < T(0) ? std::numeric_limits<T>::min() :
                            std::numeric_limits<T>::max();
        return range(start, stop, step);
    }

    template <typename T>
    auto count(T start) -> decltype(range(start, start)) {
        return count(start, T(1));
    }
}


#endif
