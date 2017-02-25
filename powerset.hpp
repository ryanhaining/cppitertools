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
  using CombinatorType = decltype(combinations(std::declval<Container&>(), 0));

  friend PowersetFn;

  Powersetter(Container&& container)
      : container_(std::forward<Container>(container)) {}

 public:
  Powersetter(Powersetter&&) = default;

  class Iterator
      : public std::iterator<std::input_iterator_tag, CombinatorType> {
   private:
    std::remove_reference_t<Container>* container_p_;
    std::size_t set_size_{};
    std::shared_ptr<CombinatorType> comb_;
    iterator_type<CombinatorType> comb_iter_;
    iterator_type<CombinatorType> comb_end_;

   public:
    Iterator(Container& container, std::size_t sz)
        : container_p_{&container},
          set_size_{sz},
          comb_{std::make_shared<CombinatorType>(combinations(container, sz))},
          comb_iter_{get_begin(*comb_)},
          comb_end_{get_end(*comb_)} {}

    Iterator& operator++() {
      ++comb_iter_;
      if (comb_iter_ == comb_end_) {
        ++set_size_;
        comb_ = std::make_shared<CombinatorType>(
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

    iterator_deref<CombinatorType> operator*() {
      return *comb_iter_;
    }

    iterator_arrow<CombinatorType> operator->() {
      apply_arrow(comb_iter_);
    }

    bool operator!=(const Iterator& other) const {
      return !(*this == other);
    }

    bool operator==(const Iterator& other) const {
      return set_size_ == other.set_size_ && comb_iter_ == other.comb_iter_;
    }
  };

  Iterator begin() {
    return {container_, 0};
  }

  Iterator end() {
    return {container_, dumb_size(container_) + 1};
  }
};

#endif
