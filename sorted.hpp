#ifndef ITER_SORTED_HPP_
#define ITER_SORTED_HPP_

#include "internal/iteratoriterator.hpp"
#include "internal/iterbase.hpp"

#include <algorithm>
#include <functional>
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
  template <typename ContainerT, typename = void>
  class SortedItersHolder {
   public:
    using IterIterWrap =
        IterIterWrapper<std::vector<iterator_type<ContainerT>>>;
    using ItIt = iterator_type<IterIterWrap>;
    using ConstItIt = void;

   private:
    ContainerT container_;
    IterIterWrap sorted_iters_;

   public:
    SortedItersHolder(ContainerT&& container, CompareFunc compare_func)
        : container_(std::forward<ContainerT>(container)) {
      // Fill the sorted_iters_ vector with an iterator to each
      // element in the container_
      for (auto iter = get_begin(container_); iter != get_end(container_);
           ++iter) {
        sorted_iters_.get().push_back(iter);
      }

      // sort by comparing the elements that the iterators point to
      std::sort(get_begin(sorted_iters_.get()), get_end(sorted_iters_.get()),
          [compare_func](
              iterator_type<Container> it1, iterator_type<Container> it2) {
            return std::invoke(compare_func, *it1, *it2);
          });
    }

    ItIt begin() {
      return sorted_iters_.begin();
    }

    ItIt end() {
      return sorted_iters_.end();
    }
  };

  template <typename ContainerT>
  class SortedItersHolder<ContainerT,
      std::void_t<decltype(std::begin(std::declval<AsConst<ContainerT>&>()))>> {
   public:
    using IterIterWrap =
        IterIterWrapper<std::vector<iterator_type<ContainerT>>>;
    using ItIt = iterator_type<IterIterWrap>;

    using ConstIterIterWrap =
        IterIterWrapper<std::vector<iterator_type<AsConst<ContainerT>>>>;
    using ConstItIt = iterator_type<ConstIterIterWrap>;

   private:
    ContainerT container_;
    mutable CompareFunc compare_func_;
    IterIterWrap sorted_iters_;
    mutable ConstIterIterWrap const_sorted_iters_;

    void populate_sorted_iters() const = delete;
    void populate_sorted_iters() {
      if (!sorted_iters_.empty()) {
        return;
      }
      // Fill the sorted_iters_ vector with an iterator to each
      // element in the container_
      for (auto iter = get_begin(container_); iter != get_end(container_);
           ++iter) {
        sorted_iters_.get().push_back(iter);
      }

      // sort by comparing the elements that the iterators point to
      std::sort(get_begin(sorted_iters_.get()), get_end(sorted_iters_.get()),
          [this](iterator_type<ContainerT> it1, iterator_type<ContainerT> it2) {
            return std::invoke(compare_func_, *it1, *it2);
          });
    }

    void populate_const_sorted_iters() = delete;
    void populate_const_sorted_iters() const {
      if (!const_sorted_iters_.empty()) {
        return;
      }
      for (auto iter = get_begin(std::as_const(container_));
           iter != get_end(std::as_const(container_)); ++iter) {
        const_sorted_iters_.get().push_back(iter);
      }

      // sort by comparing the elements that the iterators point to
      std::sort(get_begin(const_sorted_iters_.get()),
          get_end(const_sorted_iters_.get()),
          [this](iterator_type<AsConst<ContainerT>> it1,
              iterator_type<AsConst<ContainerT>> it2) {
            return compare_func_(*it1, *it2);
          });
    }

   public:
    SortedItersHolder(ContainerT&& container, CompareFunc compare_func)
        : container_(std::forward<ContainerT>(container)),
          compare_func_(std::move(compare_func)) {}

    ItIt begin() {
      populate_sorted_iters();
      return sorted_iters_.begin();
    }

    ItIt end() {
      populate_sorted_iters();
      return sorted_iters_.end();
    }

    ConstItIt begin() const {
      populate_const_sorted_iters();
      return const_sorted_iters_.begin();
    }

    ConstItIt end() const {
      populate_const_sorted_iters();
      return const_sorted_iters_.end();
    }
  };

  friend SortedFn;

  SortedItersHolder<Container> sorted_iters_holder_;

  SortedView(Container&& container, CompareFunc compare_func)
      : sorted_iters_holder_{
            std::forward<Container>(container), std::move(compare_func)} {}

 public:
  SortedView(SortedView&&) = default;

  typename SortedItersHolder<Container>::ItIt begin() {
    return sorted_iters_holder_.begin();
  }

  typename SortedItersHolder<Container>::ItIt end() {
    return sorted_iters_holder_.end();
  }

  typename SortedItersHolder<Container>::ConstItIt begin() const {
    return sorted_iters_holder_.begin();
  }

  typename SortedItersHolder<Container>::ConstItIt end() const {
    return sorted_iters_holder_.end();
  }
};

#endif
