#ifndef ITER_SORTED_HPP_
#define ITER_SORTED_HPP_

#include "internal/iterbase.hpp"
#include "internal/iteratoriterator.hpp"

#include <iterator>
#include <algorithm>
#include <vector>

namespace iter {
  namespace impl {
    template <typename Container>
    class SortedView;
  }

  template <typename Container, typename CompareFunc>
  impl::SortedView<Container> sorted(Container&&, CompareFunc);
}

template <typename Container>
class iter::impl::SortedView {
 private:
  using IterIterWrap = IterIterWrapper<std::vector<iterator_type<Container>>>;
  using ItIt = iterator_type<IterIterWrap>;

  template <typename C, typename F>
  friend SortedView<C> iter::sorted(C&&, F);

  Container container;
  IterIterWrap sorted_iters;

  template <typename CompareFunc>
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
        [compare_func](const iterator_type<Container>& it1,
                  const iterator_type<Container>& it2) {
          return compare_func(*it1, *it2);
        });
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

template <typename Container, typename CompareFunc>
iter::impl::SortedView<Container> iter::sorted(
    Container&& container, CompareFunc compare_func) {
  return {std::forward<Container>(container), compare_func};
}

namespace iter {
  template <typename Container>
  auto sorted(Container&& container)
      -> decltype(sorted(std::forward<Container>(container),
          std::less<impl::const_iterator_deref<Container>>())) {
    return sorted(std::forward<Container>(container),
        std::less<impl::const_iterator_deref<Container>>());
  }
}

#endif
