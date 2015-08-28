#ifndef ITER_SLICE_HPP_
#define ITER_SLICE_HPP_

#include "internal/iterbase.hpp"

#include <iterator>
#include <type_traits>
#include <initializer_list>

namespace iter {
  namespace impl {
    template <typename Container, typename DifferenceType>
    class Sliced;
  }

  template <typename Container, typename DifferenceType>
  impl::Sliced<Container, DifferenceType> slice(Container&& container,
      DifferenceType start, DifferenceType stop, DifferenceType step = 1);

  template <typename Container, typename DifferenceType>
  impl::Sliced<Container, DifferenceType> slice(
      Container&& container, DifferenceType stop);

  template <typename T, typename DifferenceType>
  impl::Sliced<std::initializer_list<T>, DifferenceType> slice(
      std::initializer_list<T> il, DifferenceType start, DifferenceType stop,
      DifferenceType step = 1);

  template <typename T, typename DifferenceType>
  impl::Sliced<std::initializer_list<T>, DifferenceType> slice(
      std::initializer_list<T> il, DifferenceType stop);
}

template <typename Container, typename DifferenceType>
class iter::impl::Sliced {
 private:
  Container container;
  DifferenceType start;
  DifferenceType stop;
  DifferenceType step;

  friend Sliced iter::slice<Container, DifferenceType>(
      Container&&, DifferenceType, DifferenceType, DifferenceType);

  friend Sliced iter::slice<Container, DifferenceType>(
      Container&&, DifferenceType);

  Sliced(Container&& in_container, DifferenceType in_start,
      DifferenceType in_stop, DifferenceType in_step)
      : container(std::forward<Container>(in_container)),
        start{in_start < in_stop && in_step > 0 ? in_start : in_stop},
        stop{in_stop},
        step{in_step} {}

 public:
  Sliced(Sliced&&) = default;
  class Iterator : public std::iterator<std::input_iterator_tag,
                       iterator_traits_deref<Container>> {
   private:
    iterator_type<Container> sub_iter;
    iterator_type<Container> sub_end;
    DifferenceType current;
    DifferenceType stop;
    DifferenceType step;

   public:
    Iterator(iterator_type<Container>&& si, iterator_type<Container>&& se,
        DifferenceType in_start, DifferenceType in_stop, DifferenceType in_step)
        : sub_iter{std::move(si)},
          sub_end{std::move(se)},
          current{in_start},
          stop{in_stop},
          step{in_step} {}

    iterator_deref<Container> operator*() {
      return *this->sub_iter;
    }

    iterator_arrow<Container> operator->() {
      return apply_arrow(this->sub_iter);
    }

    Iterator& operator++() {
      dumb_advance(this->sub_iter, this->sub_end, this->step);
      this->current += this->step;
      if (this->stop < this->current) {
        this->current = this->stop;
      }
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    bool operator!=(const Iterator& other) const {
      return this->sub_iter != other.sub_iter && this->current != other.current;
    }

    bool operator==(const Iterator& other) const {
      return !(*this != other);
    }
  };

  Iterator begin() {
    auto it = std::begin(this->container);
    dumb_advance(it, std::end(this->container), this->start);
    return {std::move(it), std::end(this->container), this->start, this->stop,
        this->step};
  }

  Iterator end() {
    return {std::end(this->container), std::end(this->container), this->stop,
        this->stop, this->step};
  }
};

// Helper function to instantiate a Sliced
template <typename Container, typename DifferenceType>
iter::impl::Sliced<Container, DifferenceType> iter::slice(Container&& container,
    DifferenceType start, DifferenceType stop, DifferenceType step) {
  return {std::forward<Container>(container), start, stop, step};
}

// only give the end as an arg and assume step  is 1 and begin is 0
template <typename Container, typename DifferenceType>
iter::impl::Sliced<Container, DifferenceType> iter::slice(
    Container&& container, DifferenceType stop) {
  return {std::forward<Container>(container), 0, stop, 1};
}

template <typename T, typename DifferenceType>
iter::impl::Sliced<std::initializer_list<T>, DifferenceType> iter::slice(
    std::initializer_list<T> il, DifferenceType start, DifferenceType stop,
    DifferenceType step) {
  return {std::move(il), start, stop, step};
}

template <typename T, typename DifferenceType>
iter::impl::Sliced<std::initializer_list<T>, DifferenceType> iter::slice(
    std::initializer_list<T> il, DifferenceType stop) {
  return {std::move(il), 0, stop, 1};
}

#endif
