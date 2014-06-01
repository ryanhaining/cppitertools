#ifndef FILTER_FALSE__HPP__
#define FILTER_FALSE__HPP__

#include "iterbase.hpp"
#include "filter.hpp"

#include <utility>

namespace iter {

    namespace detail {

        // Callable object that reverses the boolean result of another
        // callable, taking the object in a Container's iterator
        template <typename FilterFunc, typename Container>
        class PredicateFlipper {
            private:
                FilterFunc filter_func;

            public:
                PredicateFlipper(FilterFunc filter_func) :
                    filter_func(filter_func)
                { }

                PredicateFlipper() = delete;
                PredicateFlipper(const PredicateFlipper&) = default;

                // Calls the filter_func 
                bool operator() (const iterator_deref<Container> item) const {
                    return !bool(filter_func(item));
                }
        };

        // Reverses the bool() conversion result of anything that supports a
        // bool conversion
        template <typename Container>
        class BoolFlipper {
            public:
                bool operator() (const iterator_deref<Container> item) const {
                    return !bool(item);
                }
        };


    }

    // Creates a PredicateFlipper for the predicate function, which reverses
    // the bool result of the function.  The PredicateFlipper is then passed
    // to the normal filter() function
    template <typename FilterFunc, typename Container>
    auto filterfalse(FilterFunc filter_func, Container&& container) ->
            decltype(filter(
                        detail::PredicateFlipper<FilterFunc, Container>(
                            filter_func),
                        std::forward<Container>(container))) {
        return filter(
                detail::PredicateFlipper<FilterFunc, Container>(filter_func),
                std::forward<Container>(container));
    }

    // Single argument version, uses a BoolFlipper to reverse the truthiness
    // of an object
    template <typename Container>
    auto filterfalse(Container&& container) ->
            decltype(filter(
                        detail::BoolFlipper<Container>(),
                        std::forward<Container>(container))) {
        return filter(
                detail::BoolFlipper<Container>(),
                std::forward<Container>(container));
    }

    

    //specializations for initializer_lists
    template <typename FilterFunc, typename T>
    auto filterfalse(FilterFunc filter_func, std::initializer_list<T> container) ->
            decltype(filter(
                        detail::PredicateFlipper<FilterFunc, std::initializer_list<T>>(
                            filter_func),
                        std::move(container))) {
        return filter(
                detail::PredicateFlipper<FilterFunc, std::initializer_list<T>>(filter_func),
                std::move(container));
    }

    // Single argument version, uses a BoolFlipper to reverse the truthiness
    // of an object
    template <typename T>
    auto filterfalse(std::initializer_list<T> container) ->
            decltype(filter(
                        detail::BoolFlipper<std::initializer_list<T>>(),
                        std::move(container))) {
        return filter(
                detail::BoolFlipper<std::initializer_list<T>>(),
                std::move(container));
    }
}

#endif //#ifndef FILTER_FALSE__HPP__
