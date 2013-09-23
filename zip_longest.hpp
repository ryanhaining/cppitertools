#ifndef ZIP_LONGEST_HPP
#define ZIP_LONGEST_HPP

#include <boost/optional.hpp>
#include <tuple>
#include "iterator_range.hpp"

namespace iter {
    template <typename ... Rest>
        struct zip_longest_iter;
    template <typename ... Containers>
        auto zip_longest(Containers & ... containers) ->
        iterator_range<zip_longest_iter<decltype(containers.begin())...>>
        {
            auto begin = 
                zip_longest_iter<decltype(containers.begin())...>(containers.begin()...);
            auto end = 
                zip_longest_iter<decltype(containers.end())...>(containers.end()...);
            return iterator_range<decltype(begin)>(begin,end);
        }   
    /*
    template <int N,typename Tuple>
    auto zip_get(Tuple & t)->decltype(*std::get<N>(t))&
    {
        return *std::get<N>(t);
    }
    */
    template <typename First, typename Second>
        struct zip_longest_iter<First,Second> {

        private:
            First iter1;
            mutable bool end_iter1_reached = false;
            Second iter2;
            mutable bool end_iter2_reached = false;

        public:
            using Elem1_t = decltype(*iter1);
            using Elem2_t = decltype(*iter2);
            zip_longest_iter(const First & f, const Second & s) :
                iter1(f),iter2(s) { }

            std::tuple<boost::optional<First>,boost::optional<Second>> operator*()     
            {
                auto first = end_iter1_reached ? boost::optional<First>() : boost::optional<First>(iter1);
                auto second = end_iter2_reached ? boost::optional<Second>() : boost::optional<Second>(iter2);
                return std::make_tuple(first,second);
            }
            zip_longest_iter & operator++() {
                if(!end_iter1_reached)++iter1;
                if(!end_iter2_reached)++iter2;
                return *this;
            }
            bool operator!=(const zip_longest_iter & rhs) const {
                if (!(this->iter1 != rhs.iter1)) end_iter1_reached = true;
                if (!(this->iter2 != rhs.iter2)) end_iter2_reached = true;
                return (this->iter1 != rhs.iter1) || (this->iter2 != rhs.iter2);
            }
        };
    template <typename First, typename ... Rest>
        struct zip_longest_iter<First,Rest...> {

        private:
            First iter;
            mutable bool end_reached = false;
            zip_longest_iter<Rest...> inner_iter;
            
        public:
            using Elem_t = decltype(*iter);
            using tuple_t = 
                decltype(std::tuple_cat(std::tuple<boost::optional<First>>(),*inner_iter));

            zip_longest_iter(const First & f, const Rest & ... rest) :
                iter(f),
                inner_iter(rest...) {}

            //this is for returning a tuple of optional<iterator>

            tuple_t operator*()
            {
                return std::tuple_cat(std::make_tuple(end_reached?boost::optional<First>():boost::optional<First>(iter)),*inner_iter);
            }
            zip_longest_iter & operator++() {
                if (!end_reached) ++iter;
                ++inner_iter;
                return *this;
            }
            bool operator!=(const zip_longest_iter & rhs) const {
                if (!(this->iter != rhs.iter)) end_reached = true;
                return (this->iter != rhs.iter) || (this->inner_iter != rhs.inner_iter);
            }
        };
}
//should add reset after the end of a range is reached, just in case someone 
//tries to use it again
//this means it's only safe to use the range ONCE
#endif //ZIP_LONGEST_HPP
