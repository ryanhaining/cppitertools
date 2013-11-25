#ifndef PRODUCT_HPP
#define PRODUCT_HPP
#include "iterator_range.hpp"

#include <tuple>
#include <utility>
#include <iterator>

namespace iter {
    template <typename ... Containers>
        struct product_iter;
    template <typename ... Containers>
        iterator_range<product_iter<Containers...>>
        product(const Containers & ... containers) {
            auto begin = product_iter<Containers...>(containers...);
            auto end = product_iter<Containers...>(containers...);
            return iterator_range<decltype(begin)>(begin,end);
        }
    //template <typenam
    template <typename Container>
        struct product_iter<Container> {
            public:
                using Iterator = decltype(std::begin(std::declval<Container>()));
            private:
                Iterator begin;
                Iterator mover;
                const Iterator end;
            public:
                product_iter(const Container & c) :
                    begin(std::begin(c)),
                    mover(std::begin(c)),
                    end(std::end(c)){}
                decltype(std::make_tuple(*mover)) operator*()
                //since you can't modify anything anyway it's ok to return a
                //tuple of whatever the iterator derefs to
                {
                    return std::make_tuple(*mover);
                }
                product_iter & operator++()
                {
                    ++mover;
                    return *this;
                }
                bool is_not_empty_range() {
                    return begin != end;
                }
                bool operator!=(const product_iter&) const 
                {
                    return mover != end;
                }
                bool is_next_iteration()
                {
                    if (!(mover != end)) {
                        mover = begin;
                        return true;
                    }
                    else return false;
                }
        };

    template <typename Container, typename ...Containers>
        struct product_iter<Container,Containers...>
        {
            public:
                using Iterator = decltype(std::begin(std::declval<Container>()));
            private:
                Iterator begin;
                Iterator mover;
                const Iterator end;
                product_iter<Containers...> inner_iter;
                bool no_empty_ranges;
            public:
                using Tuple_type = decltype(std::tuple_cat(std::make_tuple(*mover),*inner_iter));
                bool is_not_empty_range() {
                    return begin != end && inner_iter.is_not_empty_range();
                }
                product_iter(const Container & c, const Containers & ... containers):
                    begin(std::begin(c)),
                    mover(std::begin(c)),
                    end(std::end(c)),
                    inner_iter(containers...){
                        no_empty_ranges = is_not_empty_range();
                    }
                Tuple_type operator*()
                {
                    return std::tuple_cat(std::make_tuple(*mover),*inner_iter);
                }
                product_iter & operator++()
                {
                    ++inner_iter;
                    if(inner_iter.is_next_iteration())
                    {
                        ++mover;
                    }
                    return *this;
                }
                bool is_next_iteration()
                {
                    if(!(mover != end)) {
                        mover = begin;
                        return true;
                    }
                    else return false;
                }
                
                bool operator!=(const product_iter&)const
                {
                    return mover != end && no_empty_ranges;
                }
                //will seg fault if anything but the first is an empty range 
                //since != only checks the first one
        };
}
namespace std {
    template <typename ... Containers>
        struct iterator_traits<iter::product_iter<Containers...>> {
            using difference_type = ptrdiff_t;
            using iterator_category = input_iterator_tag;
        };
}
#endif //PRODUCT_HPP
                    
                    

