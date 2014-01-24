#ifndef COUNT__H__
#define COUNT__H__

#include "range.hpp"

#include <limits>

namespace iter {

    using DefaultRangeType = long;

    Range<DefaultRangeType> count() {
        return range(DefaultRangeType(0),
                std::numeric_limits<DefaultRangeType>::max());
    }

    template <typename T>
    Range<T> count(T start, T step) {
        // if step is < 0, set the stop to numeric min, otherwise numeric max
        T stop = step < T(0) ? std::numeric_limits<T>::min() :
                            std::numeric_limits<T>::max();
        return range(start, stop, step);
    }

    template <typename T>
    Range<T> count(T start) {
        return range(start, T(1));
    }
}


#endif //define COUNT__H__

