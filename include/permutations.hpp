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

  template <typename T>
  using IndexVector = std::vector<IteratorWrapper<T>>;
  template <typename T>
  using Permutable = IterIterWrapper<IndexVector<T>>;

  Permuter(Container&& container)
      : container_(std::forward<Container>(container)) {}

 public:
  Permuter(Permuter&&) = default;

  template <typename ContainerT>
  class Iterator {
   private:
    template <typename>
    friend class Iterator;
    static constexpr const int COMPLETE = -1;
    static bool cmp_iters(IteratorWrapper<ContainerT> lhs,
        IteratorWrapper<ContainerT> rhs) noexcept {
      return *lhs < *rhs;
    }

    Permutable<ContainerT> working_set_;
    int steps_{};

   public:
    using iterator_category = std::input_iterator_tag;
    using value_type = Permutable<ContainerT>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    Iterator(IteratorWrapper<ContainerT>&& sub_iter,
        IteratorWrapper<ContainerT>&& sub_end)
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

    Permutable<ContainerT>& operator*() {
      return working_set_;
    }

    Permutable<ContainerT>* operator->() {
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
    return {get_begin(container_), get_end(container_)};
  }

  Iterator<Container> end() {
    return {get_end(container_), get_end(container_)};
  }

  Iterator<AsConst<Container>> begin() const {
    return {get_begin(std::as_const(container_)),
        get_end(std::as_const(container_))};
  }

  Iterator<AsConst<Container>> end() const {
    return {get_end(std::as_const(container_)),
        get_end(std::as_const(container_))};
  }
};

#endif
