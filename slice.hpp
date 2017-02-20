#ifndef ITER_SLICE_HPP_
#define ITER_SLICE_HPP_

#include "internal/iterbase.hpp"
#include "internal/iterator_wrapper.hpp"

#include <iterator>
#include <type_traits>

namespace iter {
  namespace impl {
    template <typename Container, typename DifferenceType>
    class Sliced;

    struct SliceFn;
  }
}

template <typename Container, typename DifferenceType>
class iter::impl::Sliced {
 private:
  Container container;
  DifferenceType start;
  DifferenceType stop;
  DifferenceType step;

  friend SliceFn;

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
    IteratorWrapper<Container> sub_iter;
    IteratorWrapper<Container> sub_end;
    DifferenceType current;
    DifferenceType stop;
    DifferenceType step;

   public:
    Iterator(IteratorWrapper<Container>&& si, IteratorWrapper<Container>&& se,
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

struct iter::impl::SliceFn {
 private:
  template <typename DifferenceType>
  class FnPartial : public Pipeable<FnPartial<DifferenceType>> {
   public:
    template <typename Container>
    Sliced<Container, DifferenceType> operator()(Container&& container) const {
      return {std::forward<Container>(container), start, stop, step};
    }

   private:
    friend SliceFn;
    constexpr FnPartial(DifferenceType in_start, DifferenceType in_stop,
        DifferenceType in_step) noexcept : start{in_start},
                                           stop{in_stop},
                                           step{in_step} {}
    DifferenceType start;
    DifferenceType stop;
    DifferenceType step;
  };

 public:
  template <typename Container, typename DifferenceType,
      typename = std::enable_if_t<is_iterable<Container>>>
  Sliced<Container, DifferenceType> operator()(Container&& container,
      DifferenceType start, DifferenceType stop,
      DifferenceType step = 1) const {
    return {std::forward<Container>(container), start, stop, step};
  }

  // only given the end, assume step is 1 and begin is 0
  template <typename Container, typename DifferenceType,
      typename = std::enable_if_t<is_iterable<Container>>>
  iter::impl::Sliced<Container, DifferenceType> operator()(
      Container&& container, DifferenceType stop) const {
    return {std::forward<Container>(container), 0, stop, 1};
  }

  template <typename DifferenceType,
      typename = std::enable_if_t<!is_iterable<DifferenceType>>>
  constexpr FnPartial<DifferenceType> operator()(DifferenceType stop) const
      noexcept {
    return {0, stop, 1};
  }

  template <typename DifferenceType,
      typename = std::enable_if_t<!is_iterable<DifferenceType>>>
  constexpr FnPartial<DifferenceType> operator()(DifferenceType start,
      DifferenceType stop, DifferenceType step = 1) const noexcept {
    return {start, stop, step};
  }
};

namespace iter {
  constexpr impl::SliceFn slice{};
}

#endif
