#ifndef ITER_TAKEWHILE_H_
#define ITER_TAKEWHILE_H_

#include "filter.hpp"
#include "internal/iterator_wrapper.hpp"
#include "internal/iterbase.hpp"

#include <iterator>
#include <utility>

namespace iter {
  namespace impl {
    template <typename FilterFunc, typename Container>
    class Taker;

    using TakeWhileFn = IterToolFnOptionalBindFirst<Taker, BoolTester>;
  }
  constexpr impl::TakeWhileFn takewhile{};
}

template <typename FilterFunc, typename Container>
class iter::impl::Taker {
 private:
  Container container;
  FilterFunc filter_func;

  friend TakeWhileFn;

  Taker(FilterFunc in_filter_func, Container&& in_container)
      : container(std::forward<Container>(in_container)),
        filter_func(in_filter_func) {}

 public:
  Taker(Taker&&) = default;

  class Iterator : public std::iterator<std::input_iterator_tag,
                       iterator_traits_deref<Container>> {
   private:
    using Holder = DerefHolder<iterator_deref<Container>>;
    IteratorWrapper<Container> sub_iter;
    IteratorWrapper<Container> sub_end;
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
    Iterator(IteratorWrapper<Container>&& iter,
        IteratorWrapper<Container>&& end, FilterFunc& in_filter_func)
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

#endif
