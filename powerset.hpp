#ifndef ITER_POWERSET_HPP_
#define ITER_POWERSET_HPP_

#include "combinations.hpp"
#include "internal/iterbase.hpp"

#include <cassert>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

namespace iter {
  namespace impl {
    template <typename Container>
    class Powersetter;

    using PowersetFn = IterToolFn<Powersetter>;
  }
  constexpr impl::PowersetFn powerset{};
}

template <typename Container>
class iter::impl::Powersetter {
 private:
  Container container_;
  template <typename T>
  using CombinatorType = decltype(combinations(std::declval<T&>(), 0));

  friend PowersetFn;

  Powersetter(Container&& container)
      : container_(std::forward<Container>(container)) {}

 public:
  Powersetter(Powersetter&&) = default;

  template <typename ContainerT>
  class Iterator {
   private:
#if 0
    template <typename> friend class Iterator;
#endif
    std::remove_reference_t<ContainerT>* container_p_;
    std::size_t set_size_{};
    std::shared_ptr<CombinatorType<ContainerT>> comb_;
    iterator_type<CombinatorType<ContainerT>> comb_iter_;
    iterator_type<CombinatorType<ContainerT>> comb_end_;

   public:
    using iterator_category = std::input_iterator_tag;
    using value_type = CombinatorType<ContainerT>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    Iterator(ContainerT& container, std::size_t sz)
        : container_p_{&container},
          set_size_{sz},
          comb_{std::make_shared<CombinatorType<ContainerT>>(
              combinations(container, sz))},
          comb_iter_{get_begin(*comb_)},
          comb_end_{get_end(*comb_)} {}

    Iterator& operator++() {
      ++comb_iter_;
      if (comb_iter_ == comb_end_) {
        ++set_size_;
        comb_ = std::make_shared<CombinatorType<ContainerT>>(
            combinations(*container_p_, set_size_));

        comb_iter_ = get_begin(*comb_);
        comb_end_ = get_end(*comb_);
      }
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    iterator_deref<CombinatorType<ContainerT>> operator*() {
      return *comb_iter_;
    }

    iterator_arrow<CombinatorType<ContainerT>> operator->() {
      apply_arrow(comb_iter_);
    }

    bool operator!=(const Iterator& other) const {
      return !(*this == other);
    }

    bool operator==(const Iterator& other) const {
      return set_size_ == other.set_size_ && comb_iter_ == other.comb_iter_;
    }
#if 0
    template <typename T>
    bool operator!=(const Iterator<T>& other) const {
      return !(*this == other);
    }

    template <typename T>
    bool operator==(const Iterator<T>& other) const {
      return set_size_ == other.set_size_ && comb_iter_ == other.comb_iter_;
    }
#endif
  };

  Iterator<Container> begin() {
    return {container_, 0};
  }

  Iterator<Container> end() {
    return {container_, dumb_size(container_) + 1};
  }

  Iterator<AsConst<Container>> begin() const {
    return {std::as_const(container_), 0};
  }

  Iterator<AsConst<Container>> end() const {
    return {
        std::as_const(container_), dumb_size(std::as_const(container_)) + 1};
  }
};

#endif
