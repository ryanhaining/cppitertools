#ifndef FILTER_FALSE__HPP__
#define FILTER_FALSE__HPP__

#include "filter.hpp"

namespace iter {

    namespace detail {

        // Callable object that reverses the boolean result of another
        // callable, taking the object in a Container's iterator
        template <typename FilterFunc, typename Container>
        class PredicateFlipper {
            private:
                FilterFunc filter_func;

                using contained_iter_type =
                    decltype(std::declval<Container>().begin());

                using contained_iter_ret =
                    decltype(std::declval<contained_iter_type>().operator*());

            public:
                PredicateFlipper(FilterFunc filter_func) :
                    filter_func(filter_func)
                { }

                PredicateFlipper() = delete;
                PredicateFlipper(const PredicateFlipper &) = default;

                // Calls the filter_func 
                bool operator() (const contained_iter_ret item) const {
                    return !bool(filter_func(item));
                }
        };

        // Reverses the bool() conversion result of anything that supports a
        // bool conversion
        template <typename Container>
        class BoolFlipper {
            private:
                using contained_iter_type =
                    decltype(std::declval<Container>().begin());

                using contained_iter_ret =
                    decltype(std::declval<contained_iter_type>().operator*());

            public:
                BoolFlipper() = default;
                BoolFlipper(const BoolFlipper &) = default;
                bool operator() (const contained_iter_ret item) const {
                    return !bool(item);
                }
        };


    }

    // Creates a PredicateFlipper for the predicate function, which reverses
    // the bool result of the function.  The PredicateFlipper is then passed
    // to the normal filter() function
    template <typename FilterFunc, typename Container>
    auto filterfalse(FilterFunc filter_func, Container & container) ->
            decltype(filter(detail::PredicateFlipper<FilterFunc, Container>(
                            filter_func), container)) {
        return filter(
                detail::PredicateFlipper<FilterFunc, Container>(filter_func),
                container);
    }

    // Single argument version, uses a BoolFlipper to reverse the truthiness
    // of an object
    template <typename Container>
    auto filterfalse(Container & container) ->
            decltype(filter(detail::BoolFlipper<Container>(), container)) {
        return filter(detail::BoolFlipper<Container>(), container);
    }

}

#endif //#ifndef FILTER_FALSE__HPP__
