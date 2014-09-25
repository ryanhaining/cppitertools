#ifndef ITER_FILTER_H_
#define ITER_FILTER_H_

#include "iterbase.hpp"

#include <utility>
#include <iterator>
#include <initializer_list>

namespace iter {

    //Forward declarations of Filter and filter
    template <typename FilterFunc, typename Container>
    class Filter;

    template <typename FilterFunc, typename Container>
    Filter<FilterFunc, Container> filter(FilterFunc, Container&&);

    template <typename FilterFunc, typename T>
    Filter<FilterFunc, std::initializer_list<T>> filter(
            FilterFunc, std::initializer_list<T>);

    template <typename FilterFunc, typename Container>
    class Filter {
        private:
            Container container;
            FilterFunc filter_func;

            // The filter function is the only thing allowed to create a Filter
            friend Filter filter<FilterFunc, Container>(
                    FilterFunc, Container&&);

            template <typename FF, typename T>
            friend Filter<FF, std::initializer_list<T>> filter(
                    FF, std::initializer_list<T>);
            
            // Value constructor for use only in the filter function
            Filter(FilterFunc filter_func, Container container)
                : container(std::forward<Container>(container)),
                filter_func(filter_func)
            { }
            Filter() = delete;
            Filter& operator=(const Filter&) = delete;

        public:
            Filter(const Filter&) = default;

            class Iterator {
                protected:
                    iterator_type<Container> sub_iter;
                    const iterator_type<Container> sub_end;
                    FilterFunc filter_func;

                    // increment until the iterator points to is true on the 
                    // predicate.  Called by constructor and operator++
                    void skip_failures() { 
                        while (this->sub_iter != this->sub_end
                                && !this->filter_func(*this->sub_iter)) {
                            ++this->sub_iter;
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
                        this->skip_failures();
                    } 

                    iterator_deref<Container> operator*() {
                        return *this->sub_iter;
                    }

                    Iterator& operator++() { 
                        ++this->sub_iter;
                        this->skip_failures();
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

    // Helper function to instantiate a Filter
    template <typename FilterFunc, typename Container>
    Filter<FilterFunc, Container> filter(
            FilterFunc filter_func, Container&& container) {
        return {filter_func, std::forward<Container>(container)};
    }

    template <typename FilterFunc, typename T>
    Filter<FilterFunc, std::initializer_list<T>> filter(
            FilterFunc filter_func,
            std::initializer_list<T> il)
    {
        return {filter_func, std::move(il)};
    }

    namespace detail {

        template <typename T>
        bool boolean_cast(const T& t) {
            return bool(t);
        }

        template <typename Container>
        class BoolTester {
            public:
                bool operator() (const iterator_deref<Container> item) const {
                    return bool(item);
                }
        };
    }


    template <typename Container>
    auto filter(Container&& container) ->
            decltype(filter(
                        detail::BoolTester<Container>(),
                        std::forward<Container>(container))) {
        return filter(
                detail::BoolTester<Container>(),
                std::forward<Container>(container));
    }

    template <typename T>
    auto filter(std::initializer_list<T> il) ->
            decltype(filter(
                        detail::BoolTester<std::initializer_list<T>>(),
                        std::move(il))) {
           return filter(
                   detail::BoolTester<std::initializer_list<T>>(),
                   std::move(il));
    }

}

#endif // #ifndef ITER_FILTER_H_
