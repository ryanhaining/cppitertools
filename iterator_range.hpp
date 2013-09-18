#ifndef ITERATOR_RANGE_HPP__
#define ITERATOR_RANGE_HPP__

namespace iter {

    template <typename Iterator>
    class iterator_range {
        private:
            const Iterator begin_;
            const Iterator end_;

        public:
            // TODO decide what contstructors should be enabled and disabled
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
