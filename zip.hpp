#ifndef ZIP_HPP
#define ZIP_HPP

#include <tuple>
#include "iterator_range.hpp"

namespace iter {
    template <typename ... Rest>
        struct zip_iter;
    template <typename ... Containers>
        auto zip(Containers && ... containers) ->
        iterator_range<zip_iter<decltype(containers.begin())...>>
        {
            auto begin = zip_iter<decltype(containers.begin())...>(containers.begin()...);
            auto end = zip_iter<decltype(containers.end())...>(containers.end()...);
            return iterator_range<decltype(begin)>(begin,end);
        }
    template <typename Iterator>
        struct zip_iter<Iterator> {

        private:
            Iterator iter;

        public:
            using Elem_t = decltype(*iter);
            zip_iter(const Iterator & i) :
                iter(i){ }

            auto operator*() -> decltype(std::tie(*iter))
            {
                return std::tie(*iter);
            }
            zip_iter & operator++() {
                ++iter;
                return *this;
            }
            bool operator!=(const zip_iter & rhs) const {
                return (this->iter != rhs.iter);
            }
        };  /* 
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

            auto operator*() -> decltype(std::tie(*iter1,*iter2))
            {
                return std::tie(*iter1,*iter2);
            }
            zip_iter & operator++() {
                ++iter1;
                ++iter2;
                return *this;
            }
            bool operator!=(const zip_iter & rhs) const {
                return (this->iter1 != rhs.iter1) && (this->iter2 != rhs.iter2);
            }
        };*/
    //this specialization commented out 
    template <typename First, typename ... Rest>
        struct zip_iter<First,Rest...> {

        private:
            First iter;
            zip_iter<Rest...> inner_iter;
            
        public:
            using Elem_t = decltype(*iter);
            using tuple_t = 
                decltype(std::tuple_cat(std::tie(*iter),*inner_iter));

            zip_iter(const First & f, const Rest & ... rest) :
                iter(f),
                inner_iter(rest...) {}


            tuple_t operator*()
            {
                return std::tuple_cat(std::tie(*iter),*inner_iter);
            }
            zip_iter & operator++() {
                ++iter;
                ++inner_iter;
                return *this;
            }
            bool operator!=(const zip_iter & rhs) const {
                return (this->iter != rhs.iter) && (this->inner_iter != rhs.inner_iter);
            }
        };
}

#endif //ZIP_HPP
