#ifndef ITER_TAKEWHILE_H_
#define ITER_TAKEWHILE_H_

#include "internal/iterbase.hpp"

#include <utility>
#include <iterator>
#include <initializer_list>

namespace iter {
  namespace impl {
    template <typename FilterFunc, typename Container>
    class Taker;
  }

  template <typename FilterFunc, typename Container>
  impl::Taker<FilterFunc, Container> takewhile(FilterFunc, Container&&);

  template <typename FilterFunc, typename T>
  impl::Taker<FilterFunc, std::initializer_list<T>> takewhile(
      FilterFunc, std::initializer_list<T>);
}

template <typename FilterFunc, typename Container>
class iter::impl::Taker {
 private:
  Container container;
  FilterFunc filter_func;

  friend Taker iter::takewhile<FilterFunc, Container>(FilterFunc, Container&&);

  template <typename FF, typename T>
  friend Taker<FF, std::initializer_list<T>> iter::takewhile(
      FF, std::initializer_list<T>);

  Taker(FilterFunc in_filter_func, Container&& in_container)
      : container(std::forward<Container>(in_container)),
        filter_func(in_filter_func) {}

 public:
  Taker(Taker&&) = default;

  class Iterator : public std::iterator<std::input_iterator_tag,
                       iterator_traits_deref<Container>> {
   private:
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

    void check_current() {
      if (this->sub_iter != this->sub_end
          && !(*this->filter_func)(this->item.get())) {
        this->sub_iter = this->sub_end;
      }
    }

   public:
    Iterator(iterator_type<Container>&& iter, iterator_type<Container>&& end,
        FilterFunc& in_filter_func)
        : sub_iter{std::move(iter)},
          sub_end{std::move(end)},
          filter_func(&in_filter_func) {
      if (this->sub_iter != this->sub_end) {
        this->item.reset(*this->sub_iter);
      }
      this->check_current();
    }

    typename Holder::reference operator*() {
      return this->item.get();
    }

    typename Holder::pointer operator->() {
      return this->item.get_ptr();
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
    return {std::begin(this->container), std::end(this->container),
        this->filter_func};
  }

  Iterator end() {
    return {std::end(this->container), std::end(this->container),
        this->filter_func};
  }
};

template <typename FilterFunc, typename Container>
iter::impl::Taker<FilterFunc, Container> iter::takewhile(
    FilterFunc filter_func, Container&& container) {
  return {filter_func, std::forward<Container>(container)};
}

template <typename FilterFunc, typename T>
iter::impl::Taker<FilterFunc, std::initializer_list<T>> iter::takewhile(
    FilterFunc filter_func, std::initializer_list<T> il) {
  return {filter_func, std::move(il)};
}

#endif
