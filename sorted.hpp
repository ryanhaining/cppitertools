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

  Container container_;
  IterIterWrap sorted_iters_;

  SortedView(Container&& container, CompareFunc compare_func)
      : container_(std::forward<Container>(container)) {
    // Fill the sorted_iters_ vector with an iterator to each
    // element in the container_
    for (auto iter = get_begin(container_); iter != get_end(container_);
         ++iter) {
      sorted_iters_.get().push_back(iter);
    }

    // sort by comparing the elements that the iterators point to
    std::sort(get_begin(sorted_iters_.get()), get_end(sorted_iters_.get()),
        [compare_func](iterator_type<Container> it1,
            iterator_type<Container> it2) { return compare_func(*it1, *it2); });
  }

 public:
  SortedView(SortedView&&) = default;

  ItIt begin() {
    return get_begin(sorted_iters_);
  }

  ItIt end() {
    return get_end(sorted_iters_);
  }
};

#endif
