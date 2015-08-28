#ifndef ITER_CYCLE_H_
#define ITER_CYCLE_H_

#include "internal/iterbase.hpp"

#include <utility>
#include <iterator>
#include <initializer_list>

namespace iter {
  namespace impl {
    template <typename Container>
    class Cycler;
  }

  template <typename Container>
  impl::Cycler<Container> cycle(Container&&);

  template <typename T>
  impl::Cycler<std::initializer_list<T>> cycle(std::initializer_list<T>);
}

template <typename Container>
class iter::impl::Cycler {
 private:
  friend Cycler iter::cycle<Container>(Container&&);
  template <typename T>
  friend Cycler<std::initializer_list<T>> iter::cycle(std::initializer_list<T>);

  Container container;

  Cycler(Container&& in_container)
      : container(std::forward<Container>(in_container)) {}

 public:
  Cycler(Cycler&&) = default;
  class Iterator : public std::iterator<std::input_iterator_tag,
                       iterator_traits_deref<Container>> {
   private:
    iterator_type<Container> sub_iter;
    iterator_type<Container> begin;
    iterator_type<Container> end;

   public:
    Iterator(
        const iterator_type<Container>& iter, iterator_type<Container>&& in_end)
        : sub_iter{iter}, begin{iter}, end{std::move(in_end)} {}

    iterator_deref<Container> operator*() {
      return *this->sub_iter;
    }

    iterator_arrow<Container> operator->() {
      return apply_arrow(this->sub_iter);
    }

    Iterator& operator++() {
      ++this->sub_iter;
      // reset to beginning upon reaching the end
      if (!(this->sub_iter != this->end)) {
        this->sub_iter = this->begin;
      }
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
    return {std::begin(this->container), std::end(this->container)};
  }

  Iterator end() {
    return {std::end(this->container), std::end(this->container)};
  }
};

template <typename Container>
iter::impl::Cycler<Container> iter::cycle(Container&& container) {
  return {std::forward<Container>(container)};
}

template <typename T>
iter::impl::Cycler<std::initializer_list<T>> iter::cycle(
    std::initializer_list<T> il) {
  return {std::move(il)};
}

#endif
