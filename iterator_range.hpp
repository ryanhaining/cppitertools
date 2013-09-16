#ifndef ITERATOR_RANGE_HPP
#define ITERATOR_RANGE_HPP

namespace iter {
    template <typename Iterator>
        struct iterator_range {
            //should decide at some point what contstructors should be 
            //enabled and disabled
            Iterator begin_;
            Iterator end_;
            iterator_range(Iterator begin, Iterator end) :
                begin_(begin),end_(end) {}
            Iterator begin() {
                return begin_;
            }
            Iterator end() {
                return end_;
            }
        };
};

#endif //ITERATOR_RANGE_HPP
