#ifndef ITER_COMBINATIONS_WITH_REPLACEMENT_HPP_
#define ITER_COMBINATIONS_WITH_REPLACEMENT_HPP_

#include "internal/iterbase.hpp"
#include "internal/iteratoriterator.hpp"

#include <iterator>
#include <vector>
#include <type_traits>
#include <initializer_list>

namespace iter {
  namespace impl {
    template <typename Container>
    class CombinatorWithReplacement;
  }

  template <typename Container>
  impl::CombinatorWithReplacement<Container> combinations_with_replacement(
      Container&&, std::size_t);

  template <typename T>
  impl::CombinatorWithReplacement<std::initializer_list<T>>
      combinations_with_replacement(std::initializer_list<T>, std::size_t);
}

template <typename Container>
class iter::impl::CombinatorWithReplacement {
 private:
  Container container;
  std::size_t length;

  friend CombinatorWithReplacement
  iter::combinations_with_replacement<Container>(Container&&, std::size_t);
  template <typename T>
  friend CombinatorWithReplacement<std::initializer_list<T>>
      iter::combinations_with_replacement(
          std::initializer_list<T>, std::size_t);

  CombinatorWithReplacement(Container&& in_container, std::size_t n)
      : container(std::forward<Container>(in_container)), length{n} {}

  using IndexVector = std::vector<iterator_type<Container>>;
  using CombIteratorDeref = IterIterWrapper<IndexVector>;

 public:
  CombinatorWithReplacement(CombinatorWithReplacement&&) = default;
  class Iterator
      : public std::iterator<std::input_iterator_tag, CombIteratorDeref> {
   private:
    constexpr static const int COMPLETE = -1;
    typename std::remove_reference<Container>::type* container_p;
    CombIteratorDeref indices;
    int steps;

   public:
    Iterator(Container& in_container, std::size_t n)
        : container_p{&in_container},
          indices(n, std::begin(in_container)),
          steps{(std::begin(in_container) != std::end(in_container) && n)
                    ? 0
                    : COMPLETE} {}

    CombIteratorDeref& operator*() {
      return this->indices;
    }

    CombIteratorDeref* operator->() {
      return &this->indices;
    }

    Iterator& operator++() {
      for (auto iter = indices.get().rbegin(); iter != indices.get().rend();
           ++iter) {
        ++(*iter);
        if (!(*iter != std::end(*this->container_p))) {
          if ((iter + 1) != indices.get().rend()) {
            for (auto down = iter; down != indices.get().rbegin() - 1; --down) {
              (*down) = dumb_next(*(iter + 1));
            }
          } else {
            this->steps = COMPLETE;
            break;
          }
        } else {
          // we break because none of the rest of the items
          // need to be incremented
          break;
        }
      }
      if (this->steps != COMPLETE) {
        ++this->steps;
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
      return this->steps == other.steps;
    }
  };

  Iterator begin() {
    return {this->container, this->length};
  }

  Iterator end() {
    return {this->container, 0};
  }
};

template <typename Container>
iter::impl::CombinatorWithReplacement<Container>
iter::combinations_with_replacement(Container&& container, std::size_t length) {
  return {std::forward<Container>(container), length};
}

template <typename T>
iter::impl::CombinatorWithReplacement<std::initializer_list<T>>
iter::combinations_with_replacement(
    std::initializer_list<T> il, std::size_t length) {
  return {std::move(il), length};
}

#endif
