#ifndef ITERATOR_RANGE_HPP__
#define ITERATOR_RANGE_HPP__

namespace iter {

    template <typename Iterator>
    class iterator_range {
        public:
            // TODO decide what contstructors should be enabled and disabled
            Iterator begin_;
            Iterator end_;

            iterator_range(Iterator begin, Iterator end) :
                begin_(begin),
                end_(end)
            { }

            Iterator begin() {
                return this->begin_;
            }

            Iterator end() {
                return this->end_;
            }
    };

}

#endif //ITERATOR_RANGE_HPP__
