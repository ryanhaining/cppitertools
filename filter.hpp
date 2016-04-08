#ifndef ITER_FILTER_H_
#define ITER_FILTER_H_

#include "internal/iterbase.hpp"

#include <utility>
#include <iterator>
#include <initializer_list>

namespace iter {
  namespace impl {
    template <typename FilterFunc, typename Container>
    class Filtered;

    struct BoolTester {
      template <typename T>
      constexpr bool operator()(const T& item) const {
        return bool(item);
      }
    };

    using FilterFn = IterToolFnOptionalBindFirst<Filtered, BoolTester>;
  }

  constexpr impl::FilterFn filter{};
}

template <typename FilterFunc, typename Container>
class iter::impl::Filtered {
 private:
  Container container;
  FilterFunc filter_func;

  friend FilterFn;

  // Value constructor for use only in the filter function
  Filtered(FilterFunc in_filter_func, Container&& in_container)
      : container(std::forward<Container>(in_container)),
        filter_func(in_filter_func) {}

 public:
  Filtered(Filtered&&) = default;

  class Iterator : public std::iterator<std::input_iterator_tag,
                       iterator_traits_deref<Container>> {
   protected:
    using Holder = DerefHolder<iterator_deref<Container>>;
    iterator_type<Container> sub_iter;
    iterator_type<Container> sub_end;
    Holder item;
    FilterFunc* filter_func;

    void inc_sub_iter() {
      ++this->sub_iter;
      if (this->sub_iter != this->sub_end) {
        this->item.reset(*this->sub_iter);
      }
    }

    // increment until the iterator points to is true on the
    // predicate.  Called by constructor and operator++
    void skip_failures() {
      while (this->sub_iter != this->sub_end
             && !(*this->filter_func)(this->item.get())) {
        this->inc_sub_iter();
      }
    }

   public:
    Iterator(iterator_type<Container> iter, iterator_type<Container> end,
        FilterFunc& in_filter_func)
        : sub_iter{iter}, sub_end{end}, filter_func(&in_filter_func) {
      if (this->sub_iter != this->sub_end) {
        this->item.reset(*this->sub_iter);
      }
      this->skip_failures();
    }

    typename Holder::reference operator*() {
      return this->item.get();
    }

    typename Holder::pointer operator->() {
      return this->item.get_ptr();
    }

    Iterator& operator++() {
      this->inc_sub_iter();
      this->skip_failures();
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
    return {std::begin(this->container), std::end(this->container),
        this->filter_func};
  }

  Iterator end() {
    return {std::end(this->container), std::end(this->container),
        this->filter_func};
  }
};

#endif
