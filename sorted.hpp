#ifndef ITER_SORTED_HPP_
#define ITER_SORTED_HPP_

#include "internal/iteratoriterator.hpp"
#include "internal/iterbase.hpp"

#include <algorithm>
#include <iterator>
#include <vector>

namespace iter {
  namespace impl {
    template <typename Container, typename CompareFunc>
    class SortedView;
    using SortedFn = IterToolFnOptionalBindSecond<SortedView, std::less<>>;
  }
  constexpr impl::SortedFn sorted{};
}

template <typename Container, typename CompareFunc>
class iter::impl::SortedView {
 private:
  using IterIterWrap = IterIterWrapper<std::vector<iterator_type<Container>>>;
  using ItIt = iterator_type<IterIterWrap>;

  friend SortedFn;

  Container container;
  IterIterWrap sorted_iters;

  SortedView(Container&& in_container, CompareFunc compare_func)
      : container(std::forward<Container>(in_container)) {
    // Fill the sorted_iters vector with an iterator to each
    // element in the container
    for (auto iter = std::begin(this->container);
         iter != std::end(this->container); ++iter) {
      this->sorted_iters.get().push_back(iter);
    }

    // sort by comparing the elements that the iterators point to
    std::sort(std::begin(sorted_iters.get()), std::end(sorted_iters.get()),
        [compare_func](iterator_type<Container> it1,
            iterator_type<Container> it2) { return compare_func(*it1, *it2); });
  }

 public:
  SortedView(SortedView&&) = default;

  ItIt begin() {
    return std::begin(sorted_iters);
  }

  ItIt end() {
    return std::end(sorted_iters);
  }
};

#endif
