#ifndef ITER_FILTER_FALSE_HPP_
#define ITER_FILTER_FALSE_HPP_

#include "filter.hpp"
#include "internal/iterbase.hpp"

#include <functional>
#include <utility>

namespace iter {
  namespace impl {
    // Callable object that reverses the boolean result of another
    // callable, taking the object in a Container's iterator
    template <typename FilterFunc>
    class PredicateFlipper {
     private:
      FilterFunc filter_func_;

     public:
      PredicateFlipper(FilterFunc filter_func)
          : filter_func_(std::move(filter_func)) {}

      // Calls the filter_func_
      template <typename T>
      bool operator()(const T& item) const {
        return !bool(std::invoke(filter_func_, item));
      }

      // with non-const incase FilterFunc::operator() is non-const
      template <typename T>
      bool operator()(const T& item) {
        return !bool(std::invoke(filter_func_, item));
      }
    };

    template <typename FilterFunc, typename Container>
    class FilterFalsed;

    using FilterFalseFn = IterToolFnOptionalBindFirst<FilterFalsed, BoolTester>;
  }
  constexpr impl::FilterFalseFn filterfalse{};
}

// Delegates to Filtered with PredicateFlipper<FilterFunc>
template <typename FilterFunc, typename Container>
class iter::impl::FilterFalsed
    : public Filtered<PredicateFlipper<FilterFunc>, Container> {
  friend FilterFalseFn;
  FilterFalsed(FilterFunc in_filter_func, Container&& in_container)
      : Filtered<PredicateFlipper<FilterFunc>, Container>(
            {in_filter_func}, std::forward<Container>(in_container)) {}
};

#endif
