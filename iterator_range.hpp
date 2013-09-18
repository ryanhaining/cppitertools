#ifndef ITERATOR_RANGE_HPP__
#define ITERATOR_RANGE_HPP__

namespace iter {

    template <typename Iterator>
    class iterator_range {
        public:
            // TODO decide what contstructors should be enabled and disabled
            const Iterator begin_;
            const Iterator end_;

            iterator_range(const Iterator & begin, const Iterator & end) :
                begin_(begin),
                end_(end)
            { }

            Iterator begin() const {
                return this->begin_;
            }

            Iterator end() const {
                return this->end_;
            }
    };

}

#endif //ITERATOR_RANGE_HPP__
