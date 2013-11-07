#ifndef WRAP_ITER_HPP__
#define WRAP_ITER_HPP__

#include <iterator>

namespace iter {
    template <typename Iterator>
    class wrap_iter {
        private:
            Iterator iter;
            typename std::iterator_traits<Iterator>::difference_type step;

        public:
            //using difference_type = typename std::iterator_traits<Iterator>::difference_type;
            wrap_iter(const Iterator & iter,
                    typename std::iterator_traits<Iterator>::difference_type step) : 
                iter(iter),
                step(step)
            { }

            wrap_iter & operator++() {
                std::advance(iter,step);
                return *this;
            }

            bool operator!=(const wrap_iter & rhs) const {
                return this->iter != rhs.iter;
            }

            auto operator*() const -> decltype(*iter)
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
namespace std {
template <typename Iterator>
    struct iterator_traits<iter::wrap_iter<Iterator>> {
        using difference_type = typename iterator_traits<Iterator>::difference_type;
        using iterator_category = typename iterator_traits<Iterator>::iterator_category;
        //should add the rest later for a more usable class
    };
}
#endif //WRAP_ITER_HPP__
