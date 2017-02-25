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
  Container container_;

  using IndexVector = std::vector<IteratorWrapper<Container>>;
  using Permutable = IterIterWrapper<IndexVector>;

  Permuter(Container&& container)
      : container_(std::forward<Container>(container)) {}

 public:
  Permuter(Permuter&&) = default;

  class Iterator : public std::iterator<std::input_iterator_tag, Permutable> {
   private:
    static constexpr const int COMPLETE = -1;
    static bool cmp_iters(IteratorWrapper<Container> lhs,
        IteratorWrapper<Container> rhs) noexcept {
      return *lhs < *rhs;
    }

    Permutable working_set_;
    int steps_{};

   public:
    Iterator(IteratorWrapper<Container>&& sub_iter,
        IteratorWrapper<Container>&& sub_end)
        : steps_{sub_iter != sub_end ? 0 : COMPLETE} {
      // done like this instead of using vector ctor with
      // two iterators because that causes a substitution
      // failure when the iterator is minimal
      while (sub_iter != sub_end) {
        working_set_.get().push_back(sub_iter);
        ++sub_iter;
      }
      std::sort(get_begin(working_set_.get()), get_end(working_set_.get()),
          cmp_iters);
    }

    Permutable& operator*() {
      return working_set_;
    }

    Permutable* operator->() {
      return &working_set_;
    }

    Iterator& operator++() {
      ++steps_;
      if (!std::next_permutation(get_begin(working_set_.get()),
              get_end(working_set_.get()), cmp_iters)) {
        steps_ = COMPLETE;
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
    return {get_begin(container_), get_end(container_)};
  }

  Iterator end() {
    return {get_end(container_), get_end(container_)};
  }
};

#endif
