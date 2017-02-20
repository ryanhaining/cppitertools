#ifndef ITER_CYCLE_H_
#define ITER_CYCLE_H_

#include "internal/iterbase.hpp"
#include "internal/iterator_wrapper.hpp"

#include <utility>
#include <iterator>
#include <initializer_list>

namespace iter {
  namespace impl {
    template <typename Container>
    class Cycler;

    using CycleFn = IterToolFn<Cycler>;
  }
  constexpr impl::CycleFn cycle{};
}

template <typename Container>
class iter::impl::Cycler {
 private:
  friend CycleFn;

  Container container;

  Cycler(Container&& in_container)
      : container(std::forward<Container>(in_container)) {}

 public:
  Cycler(Cycler&&) = default;
  class Iterator : public std::iterator<std::input_iterator_tag,
                       iterator_traits_deref<Container>> {
   private:
    IteratorWrapper<Container> sub_iter;
    IteratorWrapper<Container> begin;
    IteratorWrapper<Container> end;

   public:
    Iterator(
        IteratorWrapper<Container>&& iter, IteratorWrapper<Container>&& in_end)
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

#endif
