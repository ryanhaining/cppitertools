#ifndef ZIP_HPP
#define ZIP_HPP

#include "iterator_range.hpp"

#include <tuple>
#include <iterator>

namespace iter {
    template <typename ... Rest>
    class zip_iter;

    template <typename ... Containers>
    auto zip(Containers && ... containers) ->
            iterator_range<zip_iter<decltype(std::begin(containers))...>>
    {
        auto begin =
            zip_iter<decltype(std::begin(containers))...>(std::begin(containers)...);

        auto end =
            zip_iter<decltype(std::begin(containers))...>(std::end(containers)...);

        return {begin,end};
    }


    template <typename Iterator>
    class zip_iter<Iterator> {
        private:
            Iterator iter;

        public:
            using elem_type = decltype(*iter);
            zip_iter(const Iterator & i) :
                iter(i){ }

            auto operator*() -> decltype(std::forward_as_tuple(*iter))
            {
                return std::forward_as_tuple(*iter);
            }
            zip_iter & operator++() {
                ++iter;
                return *this;
            }
            bool operator!=(const zip_iter & rhs) const {
                return (this->iter != rhs.iter);
            }
    };
#if 0
    template <typename First, typename Second>
        struct zip_iter<First,Second> {

        private:
            First iter1;
            Second iter2;

        public:
            using Elem1_t = decltype(*iter1);
            using Elem2_t = decltype(*iter2);
            zip_iter(const First & f, const Second & s) :
                iter1(f),iter2(s) { }

            auto operator*() -> decltype(std::forward_as_tuple(*iter1,*iter2))
            {
                return std::forward_as_tuple(*iter1,*iter2);
            }
            zip_iter & operator++() {
                ++iter1;
                ++iter2;
                return *this;
            }
            bool operator!=(const zip_iter & rhs) const {
                return (this->iter1 != rhs.iter1) && (this->iter2 != rhs.iter2);
            }
        };
#endif
    //this specialization commented out 
    template <typename First, typename ... Rest>
    class zip_iter<First,Rest...> {
        private:
            First iter;
            zip_iter<Rest...> inner_iter;
            
        public:
            using elem_type = decltype(*iter);
            using tuple_type = 
                decltype(std::tuple_cat(std::forward_as_tuple(*iter),*inner_iter));

            zip_iter(const First & f, const Rest & ... rest) :
                iter(f),
                inner_iter(rest...) {}


            tuple_type operator*()
            {
                return std::tuple_cat(std::forward_as_tuple(*iter),*inner_iter);
            }

            zip_iter & operator++() {
                ++iter;
                ++inner_iter;
                return *this;
            }

            bool operator!=(const zip_iter & rhs) const {
                return (this->iter != rhs.iter) &&
                    (this->inner_iter != rhs.inner_iter);
            }
    };
}

namespace std {
    template <typename ... Containers>
        struct iterator_traits<iter::zip_iter<Containers...>> {
            using difference_type = ptrdiff_t;
            using iterator_category = input_iterator_tag;
        };
}
#endif //ZIP_HPP
