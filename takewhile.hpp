#ifndef TAKEWHILE__H__
#define TAKEWHILE__H__

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

            // Value constructor for use only in the takewhile function
            TakeWhile(FilterFunc filter_func, Container container)
                : container(std::forward<Container>(container)),
                filter_func(filter_func)
            { }

            TakeWhile () = delete;
            TakeWhile& operator=(const TakeWhile&) = delete;

        public:
            TakeWhile(const TakeWhile&) = default;

            class Iterator {
                private:
                    using iter_type = iterator_type<Container>;
                    iterator_type<Container> sub_iter;
                    const iterator_type<Container> sub_end;
                    FilterFunc filter_func;

                    // check if the current value is true under the predicate
                    // if it is not, set the sub_iter to the end using
                    // placement new to avoid the requirement of the iterator
                    // having an operator=
                    void check_current() {
                        if (!this->filter_func(*this->sub_iter)) {
                            this->sub_iter.~iter_type();
                            new(&this->sub_iter) iterator_type<Container>(
                                    this->sub_end);
                        }
                    }

                public:
                    Iterator (iterator_type<Container> iter,
                            iterator_type<Container> end,
                            FilterFunc filter_func)
                        : sub_iter{iter},
                        sub_end{end},
                        filter_func(filter_func)
                    { 
                        if (this->sub_iter != this->sub_end) {
                            // only do the check if not already at the end
                            this->check_current();
                        }
                    } 

                    iterator_deref<Container> operator*() const {
                        return *this->sub_iter;
                    }

                    Iterator& operator++() { 
                        ++this->sub_iter;
                        this->check_current();
                        return *this;
                    }

                    bool operator!=(const Iterator& other) const {
                        return this->sub_iter != other.sub_iter;
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

    // Helper function to instantiate a TakeWhile
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

#endif //ifndef TAKEWHILE__H__
