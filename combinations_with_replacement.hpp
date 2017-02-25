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

  using IndexVector = std::vector<iterator_type<Container>>;
  using CombIteratorDeref = IterIterWrapper<IndexVector>;

 public:
  CombinatorWithReplacement(CombinatorWithReplacement&&) = default;
  class Iterator
      : public std::iterator<std::input_iterator_tag, CombIteratorDeref> {
   private:
    constexpr static const int COMPLETE = -1;
    std::remove_reference_t<Container>* container_p_;
    CombIteratorDeref indices_;
    int steps_;

   public:
    Iterator(Container& in_container, std::size_t n)
        : container_p_{&in_container},
          indices_(n, get_begin(in_container)),
          steps_{(get_begin(in_container) != get_end(in_container) && n)
                     ? 0
                     : COMPLETE} {}

    CombIteratorDeref& operator*() {
      return indices_;
    }

    CombIteratorDeref* operator->() {
      return &indices_;
    }

    Iterator& operator++() {
      for (auto iter = indices_.get().rbegin(); iter != indices_.get().rend();
           ++iter) {
        ++(*iter);
        if (!(*iter != get_end(*container_p_))) {
          if ((iter + 1) != indices_.get().rend()) {
            for (auto down = iter; down != indices_.get().rbegin() - 1;
                 --down) {
              (*down) = dumb_next(*(iter + 1));
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

    bool operator!=(const Iterator& other) const {
      return !(*this == other);
    }

    bool operator==(const Iterator& other) const {
      return steps_ == other.steps_;
    }
  };

  Iterator begin() {
    return {container_, length_};
  }

  Iterator end() {
    return {container_, 0};
  }
};

#endif
