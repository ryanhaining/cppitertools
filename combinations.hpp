#ifndef ITER_COMBINATIONS_HPP_
#define ITER_COMBINATIONS_HPP_

#include "internal/iteratoriterator.hpp"
#include "internal/iterbase.hpp"

#include <iterator>
#include <type_traits>
#include <vector>

namespace iter {
  namespace impl {
    template <typename Container>
    class Combinator;

    using CombinationsFn = IterToolFnBindSizeTSecond<Combinator>;
  }
  constexpr impl::CombinationsFn combinations{};
}

template <typename Container>
class iter::impl::Combinator {
 private:
  Container container_;
  std::size_t length_;

  friend CombinationsFn;

  Combinator(Container&& container, std::size_t length)
      : container_(std::forward<Container>(container)), length_{length} {}

  template <typename T>
  using IndexVector = std::vector<iterator_type<T>>;
  template <typename T>
  using CombIteratorDeref = IterIterWrapper<IndexVector<T>>;

 public:
  Combinator(Combinator&&) = default;
  template <typename ContainerT>
  class Iterator {
   private:
    template <typename>
    friend class Iterator;
    constexpr static const int COMPLETE = -1;
    std::remove_reference_t<ContainerT>* container_p_;
    CombIteratorDeref<ContainerT> indices_;
    int steps_{};

   public:
    using iterator_category = std::input_iterator_tag;
    using value_type = CombIteratorDeref<ContainerT>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    Iterator(ContainerT& container, std::size_t n)
        : container_p_{&container}, indices_{n} {
      if (n == 0) {
        steps_ = COMPLETE;
        return;
      }
      size_t inc = 0;
      for (auto& iter : indices_.get()) {
        auto it = get_begin(*container_p_);
        dumb_advance(it, get_end(*container_p_), inc);
        if (it != get_end(*container_p_)) {
          iter = it;
          ++inc;
        } else {
          steps_ = COMPLETE;
          break;
        }
      }
    }

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

        // what we have to check here is if the distance between
        // the index and the end of indices_ is >= the distance
        // between the item and end of item
        auto dist = std::distance(indices_.get().rbegin(), iter);

        if (!(dumb_next(*iter, dist) != get_end(*container_p_))) {
          if ((iter + 1) != indices_.get().rend()) {
            size_t inc = 1;
            for (auto down = iter; ; --down) {
              (*down) = dumb_next(*(iter + 1), 1 + inc);
              ++inc;
              if (down == indices_.get().rbegin())
                break;
            }
          } else {
            steps_ = COMPLETE;
            break;
          }
        } else {
          break;
        }
        // we break because none of the rest of the items need
        // to be incremented
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
