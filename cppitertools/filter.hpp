#ifndef ITER_FILTER_H_
#define ITER_FILTER_H_

#include "internal/iterator_wrapper.hpp"
#include "internal/iterbase.hpp"

#include <functional>
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
  mutable FilterFunc filter_func_;

  friend FilterFn;

 protected:
  // Value constructor for use only in the filter function
  Filtered(FilterFunc filter_func, Container&& container)
      : container_(std::forward<Container>(container)),
        filter_func_(filter_func) {}

 public:
  Filtered(Filtered&&) = default;

  template <typename ContainerT>
  class Iterator {
   private:
    template <typename>
    friend class Iterator;
    using Holder = DerefHolder<iterator_deref<ContainerT>>;
    mutable IteratorWrapper<ContainerT> sub_iter_;
    IteratorWrapper<ContainerT> sub_end_;
    mutable Holder item_;
    FilterFunc* filter_func_;

    // All of these are marked const because the sub_iter_ is lazily
    // initialized. The morality of this is questionable.
    void inc_sub_iter() const {
      ++sub_iter_;
      if (sub_iter_ != sub_end_) {
        item_.reset(*sub_iter_);
      }
    }

    // increment until the iterator points to is true on the
    // predicate.  Called by constructor and operator++
    void skip_failures() const {
      while (
          sub_iter_ != sub_end_ && !std::invoke(*filter_func_, item_.get())) {
        inc_sub_iter();
      }
    }

    void init_if_first_use() const {
      if (!item_ && sub_iter_ != sub_end_) {
        item_.reset(*sub_iter_);
        skip_failures();
      }
    }

   public:
    using iterator_category = std::input_iterator_tag;
    using value_type = iterator_traits_deref<ContainerT>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    Iterator(IteratorWrapper<ContainerT>&& sub_iter,
        IteratorWrapper<ContainerT>&& sub_end, FilterFunc& filter_func)
        : sub_iter_{std::move(sub_iter)},
          sub_end_{std::move(sub_end)},
          filter_func_(&filter_func) {}

    typename Holder::reference operator*() {
      init_if_first_use();
      return item_.get();
    }

    typename Holder::pointer operator->() {
      init_if_first_use();
      return item_.get_ptr();
    }

    Iterator& operator++() {
      init_if_first_use();
      inc_sub_iter();
      skip_failures();
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    template <typename T>
    bool operator!=(const Iterator<T>& other) const {
      init_if_first_use();
      other.init_if_first_use();
      return sub_iter_ != other.sub_iter_;
    }

    template <typename T>
    bool operator==(const Iterator<T>& other) const {
      return !(*this != other);
    }
  };

  Iterator<Container> begin() {
    return {get_begin(container_), get_end(container_), filter_func_};
  }

  Iterator<Container> end() {
    return {get_end(container_), get_end(container_), filter_func_};
  }

  Iterator<AsConst<Container>> begin() const {
    return {get_begin(std::as_const(container_)),
        get_end(std::as_const(container_)), filter_func_};
  }

  Iterator<AsConst<Container>> end() const {
    return {get_end(std::as_const(container_)),
        get_end(std::as_const(container_)), filter_func_};
  }
};

#endif
