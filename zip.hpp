#ifndef ZIP_HPP
#define ZIP_HPP

#include <tuple>
#include "iterator_range.hpp"

namespace iter {
    template <typename ... Rest>
        struct zip_iter;
    template <typename ... Containers>
        auto zip(Containers & ... containers) ->
        iterator_range<zip_iter<decltype(containers.begin())...>>
        {
            auto begin = zip_iter<decltype(containers.begin())...>(containers.begin()...);
            auto end = zip_iter<decltype(containers.end())...>(containers.end()...);
            return iterator_range<decltype(begin)>(begin,end);
        }   template <int N,typename Tuple>
    auto zip_get(Tuple & t)->decltype(*std::get<N>(t))&
    {
        return *std::get<N>(t);
    }
        template <typename First, typename Second>
        struct zip_iter<First,Second> {
            First iter1;
            Second iter2;
            using Elem1_t = decltype(*iter1);
            using Elem2_t = decltype(*iter2);
            zip_iter(const First & f, const Second & s) :
                iter1(f),iter2(s) { }

            auto operator*() -> decltype(std::make_tuple(iter1,iter2))
            {
                return std::make_tuple(iter1,iter2);
            }

            /*
               auto  operator*() -> decltype(std::make_tuple(*iter1,*iter2))
               {
               return std::make_tuple(*iter1,*iter2);
               }
               */
            zip_iter & operator++() {
                ++iter1;
                ++iter2;
                return *this;
            }
            bool operator!=(const zip_iter & rhs) {
                return (this->iter1 != rhs.iter1) && (this->iter2 != rhs.iter2);
            }
        };
    template <typename First, typename ... Rest>
        struct zip_iter<First,Rest...> {
            First iter;
            using Elem_t = decltype(*iter);
            //Elem_t elem;
            zip_iter<Rest...> inner_iter;
            zip_iter(const First & f, const Rest & ... rest) :
                iter(f),
                inner_iter(rest...) {}

            //this is for returning a tuple of iterators
            using tuple_t = 
                decltype(std::tuple_cat(std::make_tuple(iter),*inner_iter));

            tuple_t operator*() {
                return std::tuple_cat(std::make_tuple(iter),*inner_iter);
            }

            /*
               using tuple_t = 
               decltype(std::tuple_cat(std::make_tuple(*iter),*inner_iter));
            //data will not be changed because these are not references

            tuple_t operator*() {
            return std::tuple_cat(std::make_tuple(*iter),*inner_iter);
            };
            */
            zip_iter & operator++() {
                ++iter;
                ++inner_iter;
                return *this;
            }
            bool operator!=(const zip_iter & rhs) {
                return (this->iter != rhs.iter) && (this->inner_iter != rhs.inner_iter);
            }
        };
}

#endif //ZIP_HPP
