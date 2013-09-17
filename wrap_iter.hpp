#ifndef WRAP_ITER_HPP
#define WRAP_ITER_HPP

namespace iter {
    template <typename Iterator>
        struct wrap_iter {
            Iterator iter;
            typename std::iterator_traits<Iterator>::difference_type step;
            wrap_iter(const Iterator & iter,
                    typename std::iterator_traits<Iterator>::difference_type step) : 
                iter(iter),
                step(step) {}
            wrap_iter & operator++() {
                iter += step;
                return *this;
            }
            bool operator!=(const wrap_iter & rhs) {
                return this->iter != rhs.iter;
            }
            auto operator*() -> decltype(*iter)
            {
                return *iter;
            }
        };
    template <typename Iterator>
        wrap_iter<Iterator> make_wrap_iter(const Iterator & iter,
                typename std::iterator_traits<Iterator>::difference_type step) {
            return wrap_iter<Iterator>(iter,step);
        }
}

#endif //WRAP_ITER_HPP
