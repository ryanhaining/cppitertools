#ifndef ITER_FILTER_H_
#define ITER_FILTER_H_

#include "internal/iterator_wrapper.hpp"
#include "internal/iterbase.hpp"

#include <initializer_list>
#include <iterator>
#include <utility>

namespace iter {
  namespace impl {
    template <typename FilterFunc, typename Container>
    class Filtered;

    struct BoolTester {
      template <typename T>
      constexpr bool operator()(const T& item_) const {
        return bool(item_);
      }
    };

    using FilterFn = IterToolFnOptionalBindFirst<Filtered, BoolTester>;
  }

  constexpr impl::FilterFn filter{};
}

template <typename FilterFunc, typename Container>
class iter::impl::Filtered {
 private:
  Container container_;
  FilterFunc filter_func_;

  friend FilterFn;

 protected:
  // Value constructor for use only in the filter function
  Filtered(FilterFunc filter_func, Container&& container)
      : container_(std::forward<Container>(container)),
        filter_func_(filter_func) {}

 public:
  Filtered(Filtered&&) = default;

  class Iterator : public std::iterator<std::input_iterator_tag,
                       iterator_traits_deref<Container>> {
   protected:
    using Holder = DerefHolder<iterator_deref<Container>>;
    IteratorWrapper<Container> sub_iter_;
    IteratorWrapper<Container> sub_end_;
    Holder item_;
    FilterFunc* filter_func_;

    void inc_sub_iter() {
      ++sub_iter_;
      if (sub_iter_ != sub_end_) {
        item_.reset(*sub_iter_);
      }
    }

    // increment until the iterator points to is true on the
    // predicate.  Called by constructor and operator++
    void skip_failures() {
      while (sub_iter_ != sub_end_ && !(*filter_func_)(item_.get())) {
        inc_sub_iter();
      }
    }

   public:
    Iterator(IteratorWrapper<Container>&& sub_iter,
        IteratorWrapper<Container>&& sub_end, FilterFunc& filter_func)
        : sub_iter_{std::move(sub_iter)},
          sub_end_{std::move(sub_end)},
          filter_func_(&filter_func) {
      if (sub_iter_ != sub_end_) {
        item_.reset(*sub_iter_);
      }
      skip_failures();
    }

    typename Holder::reference operator*() {
      return item_.get();
    }

    typename Holder::pointer operator->() {
      return item_.get_ptr();
    }

    Iterator& operator++() {
      inc_sub_iter();
      skip_failures();
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    bool operator!=(const Iterator& other) const {
      return sub_iter_ != other.sub_iter_;
    }

    bool operator==(const Iterator& other) const {
      return !(*this != other);
    }
  };

  Iterator begin() {
    return {get_begin(container_), get_end(container_), filter_func_};
  }

  Iterator end() {
    return {get_end(container_), get_end(container_), filter_func_};
  }
};

#endif
