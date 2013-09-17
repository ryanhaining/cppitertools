#ifndef REPEAT_HPP
#define REPEAT_HPP

#include <iostream>
#include <iterator_range.hpp>
#include <iterator>

namespace iter {
    template <typename Elem>
        struct repeat_iter {
            const Elem elem;
            size_t until;
            size_t start=0;
            repeat_iter(const Elem & elem,
                    size_t until) : 
                elem(elem),
                until(until) {}
            repeat_iter & operator++() {
                if (until != 0) ++start; //no point in repeating 0 times
                //so use that to repeat infinitely
                return *this;
            }
            bool operator!=(const repeat_iter & rhs) {
                if (until == 0) return true;
                else if (start != until) return true;
                else return false;
            }
            Elem operator*()
            {
                return elem;
            }
        };
    template <typename Elem>
        iterator_range<repeat_iter<Elem>> repeat(
                const Elem & elem,
                size_t until=0)
        //-1 causes it to repeat infintely 
        {
            return iterator_range<repeat_iter<Elem>>(
                    repeat_iter<Elem>(elem,until),
                    repeat_iter<Elem>(elem,until));//just a dummy iter
        }
        
}

#endif //REPEAT_HPP
