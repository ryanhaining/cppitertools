#ifndef ITER_TAKEWHILE_H_
#define ITER_TAKEWHILE_H_

#include "iterbase.hpp"

#include <utility>
#include <iterator>
#include <initializer_list>

namespace iter {

    //Forward declarations of TakeWhile and takewhile
    template <typename FilterFunc, typename Container>
    class TakeWhile;

    template <typename FilterFunc, typename Container>
    TakeWhile<FilterFunc, Container> takewhile(FilterFunc, Container&&);

    template <typename FilterFunc, typename T>
    TakeWhile<FilterFunc, std::initializer_list<T>> takewhile(
            FilterFunc, std::initializer_list<T>);

    template <typename FilterFunc, typename Container>
    class TakeWhile {
        private:
            Container container;
            FilterFunc filter_func;

            friend TakeWhile takewhile<FilterFunc, Container>(
                    FilterFunc, Container&&);

            template <typename FF, typename T>
            friend TakeWhile<FF, std::initializer_list<T>> takewhile(
                    FF, std::initializer_list<T>);

            TakeWhile(FilterFunc filter_func, Container&& container)
                : container(std::forward<Container>(container)),
                filter_func(filter_func)
            { }


        public:

            class Iterator 
                : public std::iterator<std::input_iterator_tag,
                    iterator_traits_deref<Container>>
            {
                private:
                    iterator_type<Container> sub_iter;
                    iterator_type<Container> sub_end;
                    DerefHolder<iterator_deref<Container>> item;
                    FilterFunc *filter_func;

                    void inc_sub_iter() {
                        ++this->sub_iter;
                        if (this->sub_iter != this->sub_end) {
                            this->item.reset(*this->sub_iter);
                        }
                    }

                    void check_current() {
                        if (this->sub_iter != this->sub_end
                                && !(*this->filter_func)(this->item.get())) {
                            this->sub_iter = this->sub_end;
                        }
                    }

                public:
                    Iterator(iterator_type<Container>&& iter,
                            iterator_type<Container>&& end,
                            FilterFunc& filter_func)
                        : sub_iter{std::move(iter)},
                        sub_end{std::move(end)},
                        filter_func(&filter_func)
                    { 
                        if (this->sub_iter != this->sub_end) {
                            this->item.reset(*this->sub_iter);
                        }
                        this->check_current();
                    } 

                    iterator_deref<Container> operator*() {
                        return this->item.pull();
                    }

                    Iterator& operator++() { 
                        this->inc_sub_iter();
                        this->check_current();
                        return *this;
                    }

                    Iterator operator++(int) {
                        auto ret = *this;
                        ++*this;
                        return ret;
                    }

                    bool operator!=(const Iterator& other) const {
                        return this->sub_iter != other.sub_iter;
                    }

                    bool operator==(const Iterator& other) const {
                        return !(*this != other);
                    }
                
            };

            Iterator begin() {
                return {std::begin(this->container),
                        std::end(this->container),
                        this->filter_func};
            }

            Iterator end() {
                return {std::end(this->container),
                        std::end(this->container),
                        this->filter_func};
            }

    };

    template <typename FilterFunc, typename Container>
    TakeWhile<FilterFunc, Container> takewhile(
            FilterFunc filter_func, Container&& container) {
        return {filter_func, std::forward<Container>(container)};
    }

    template <typename FilterFunc, typename T>
    TakeWhile<FilterFunc, std::initializer_list<T>> takewhile(
            FilterFunc filter_func, std::initializer_list<T> il)
    {
        return {filter_func, std::move(il)};
    }

}

#endif
