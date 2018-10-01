#ifndef ITER_COMBINATIONS_WITH_REPLACEMENT_HPP_
#define ITER_COMBINATIONS_WITH_REPLACEMENT_HPP_

#include "internal/iteratoriterator.hpp"
#include "internal/iterbase.hpp"

#include <iterator>
#include <type_traits>
#include <vector>

namespace iter {
  namespace impl {
    template <typename Container>
    class CombinatorWithReplacement;
    using CombinationsWithReplacementFn =
        IterToolFnBindSizeTSecond<CombinatorWithReplacement>;
  }
  constexpr impl::CombinationsWithReplacementFn combinations_with_replacement{};
}

template <typename Container>
class iter::impl::CombinatorWithReplacement {
 private:
  Container container_;
  std::size_t length_;

  friend CombinationsWithReplacementFn;

  CombinatorWithReplacement(Container&& container, std::size_t n)
      : container_(std::forward<Container>(container)), length_{n} {}

  template <typename T>
  using IndexVector = std::vector<iterator_type<T>>;
  template <typename T>
  using CombIteratorDeref = IterIterWrapper<IndexVector<T>>;

 public:
  CombinatorWithReplacement(CombinatorWithReplacement&&) = default;
  template <typename ContainerT>
  class Iterator {
   private:
    template <typename>
    friend class Iterator;
    constexpr static const int COMPLETE = -1;
    std::remove_reference_t<ContainerT>* container_p_;
    CombIteratorDeref<ContainerT> indices_;
    int steps_;

   public:
    using iterator_category = std::input_iterator_tag;
    using value_type = CombIteratorDeref<ContainerT>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    Iterator(ContainerT& in_container, std::size_t n)
        : container_p_{&in_container},
          indices_(n, get_begin(in_container)),
          steps_{(get_begin(in_container) != get_end(in_container) && n)
                     ? 0
                     : COMPLETE} {}

    CombIteratorDeref<ContainerT>& operator*() {
      return indices_;
    }

    CombIteratorDeref<ContainerT>* operator->() {
      return &indices_;
    }

    Iterator& operator++() {
      for (auto iter = indices_.get().rbegin(); iter != indices_.get().rend();
           ++iter) {
        ++(*iter);
        if (!(*iter != get_end(*container_p_))) {
          if ((iter + 1) != indices_.get().rend()) {
            for (auto down = iter; ; --down) {
              (*down) = dumb_next(*(iter + 1));
              if (down == indices_.get().rbegin())
                break;
            }
          } else {
            steps_ = COMPLETE;
            break;
          }
        } else {
          // we break because none of the rest of the items
          // need to be incremented
          break;
        }
      }
      if (steps_ != COMPLETE) {
        ++steps_;
      }
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    template <typename T>
    bool operator!=(const Iterator<T>& other) const {
      return !(*this == other);
    }

    template <typename T>
    bool operator==(const Iterator<T>& other) const {
      return steps_ == other.steps_;
    }
  };

  Iterator<Container> begin() {
    return {container_, length_};
  }

  Iterator<Container> end() {
    return {container_, 0};
  }

  Iterator<AsConst<Container>> begin() const {
    return {std::as_const(container_), length_};
  }

  Iterator<AsConst<Container>> end() const {
    return {std::as_const(container_), 0};
  }
};

#endif
