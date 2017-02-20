#ifndef ITER_PERMUTATIONS_HPP_
#define ITER_PERMUTATIONS_HPP_

#include "internal/iterator_wrapper.hpp"
#include "internal/iteratoriterator.hpp"
#include "internal/iterbase.hpp"

#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <utility>
#include <vector>

namespace iter {
  namespace impl {
    template <typename Container>
    class Permuter;
    using PermutationsFn = IterToolFn<Permuter>;
  }
  constexpr impl::PermutationsFn permutations{};
}

template <typename Container>
class iter::impl::Permuter {
 private:
  friend PermutationsFn;
  Container container;

  using IndexVector = std::vector<IteratorWrapper<Container>>;
  using Permutable = IterIterWrapper<IndexVector>;

  Permuter(Container&& in_container)
      : container(std::forward<Container>(in_container)) {}

 public:
  Permuter(Permuter&&) = default;

  class Iterator : public std::iterator<std::input_iterator_tag, Permutable> {
   private:
    static constexpr const int COMPLETE = -1;
    static bool cmp_iters(IteratorWrapper<Container> lhs,
        IteratorWrapper<Container> rhs) noexcept {
      return *lhs < *rhs;
    }

    Permutable working_set;
    int steps{};

   public:
    Iterator(IteratorWrapper<Container>&& sub_iter,
        IteratorWrapper<Container>&& sub_end)
        : steps{sub_iter != sub_end ? 0 : COMPLETE} {
      // done like this instead of using vector ctor with
      // two iterators because that causes a substitution
      // failure when the iterator is minimal
      while (sub_iter != sub_end) {
        this->working_set.get().push_back(sub_iter);
        ++sub_iter;
      }
      std::sort(std::begin(working_set.get()), std::end(working_set.get()),
          cmp_iters);
    }

    Permutable& operator*() {
      return this->working_set;
    }

    Permutable* operator->() {
      return &this->working_set;
    }

    Iterator& operator++() {
      ++this->steps;
      if (!std::next_permutation(std::begin(working_set.get()),
              std::end(working_set.get()), cmp_iters)) {
        this->steps = COMPLETE;
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
    return {std::begin(this->container), std::end(this->container)};
  }

  Iterator end() {
    return {std::end(this->container), std::end(this->container)};
  }
};

#endif
