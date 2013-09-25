#ifndef PRODUCT_HPP
#include <tuple>
//#include <type_traits>
#include "iterator_range.hpp"

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
    template <typename Container>
        struct product_iter<Container> {
            public:
                using Iterator = decltype(((Container*)nullptr)->cbegin());
            private:
                Iterator begin;
                Iterator mover;
                const Iterator end;
            public:
                product_iter(const Container & c) :
                    begin(c.cbegin()),
                    mover(c.cbegin()),
                    end(c.cend()){}
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
                bool operator!=(const product_iter&) const 
                {
                    return mover != end;
                }
                //bool is_begin() const {return mover == end;}
                bool is_next_iteration()
                {
                    if (!(mover != end)) {
                        //++begin;
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
                using Iterator = decltype(((Container*)nullptr)->cbegin());
            private:
                Iterator begin;
                Iterator mover;
                const Iterator end;
                product_iter<Containers...> inner_iter;
            public:
                using Tuple_type = decltype(std::tuple_cat(std::make_tuple(*mover),*inner_iter));
                product_iter(const Container & c, const Containers & ... containers):
                    begin(c.cbegin()),
                    mover(c.cbegin()),
                    end(c.cend()),
                    inner_iter(containers...){}
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
                        //++begin;
                        mover = begin;
                        return true;
                    }
                    else return false;
                }
                /*
                bool is_begin() const{
                    return mover == end && inner_iter.is_begin();
                }
                */
                bool operator!=(const product_iter)const
                {
                    //return begin != end || (this->inner_iter != rhs.inner_iter);
                    return mover != end;
                }
        };
}

#endif //PRODUCT_HPP
                    
                    

