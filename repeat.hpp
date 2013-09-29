#ifndef REPEAT_HPP
#define REPEAT_HPP

#include "iterator_range.hpp"

#include <iterator>

namespace iter {
    template <typename Elem>
    class repeat_iter {
        private:
            const Elem elem;
            const size_t until;
            size_t start=0;
        public:
            repeat_iter(const Elem & elem, size_t until) : 
                elem(elem),
                until(until)
            { }

            repeat_iter & operator++() {
                if (this->until != 0) ++start; //no point in repeating 0 times
                //so use that to repeat infinitely
                return *this;
            }

            bool operator!=(const repeat_iter &) const {
                return this->until == 0 || this->start != this->until;
            }

            Elem operator*() const {
                return this->elem;
            }
    };

    //-1 causes it to repeat infintely 
    template <typename Elem>
    iterator_range<repeat_iter<Elem>> repeat(
            const Elem & elem,
            size_t until=0)
    {
        return iterator_range<repeat_iter<Elem>>(
                repeat_iter<Elem>(elem, until),
                repeat_iter<Elem>(elem, until));//just a dummy iter
    }
        
}

#endif //REPEAT_HPP
