#ifndef ITER_ACCUMULATE_H_
#define ITER_ACCUMULATE_H_

#include "internal/iterator_wrapper.hpp"
#include "internal/iterbase.hpp"

#include <functional>
#include <iterator>
#include <optional>
#include <type_traits>
#include <utility>

namespace iter {
  namespace impl {
    template <typename Container, typename AccumulateFunc>
    class Accumulator;

    using AccumulateFn = IterToolFnOptionalBindSecond<Accumulator, std::plus<>>;
  }
  constexpr impl::AccumulateFn accumulate{};
}

template <typename Container, typename AccumulateFunc>
class iter::impl::Accumulator {
 private:
  Container container_;
  mutable AccumulateFunc accumulate_func_;

  friend AccumulateFn;

  using AccumVal = std::remove_reference_t<std::invoke_result_t<AccumulateFunc,
      iterator_deref<Container>, iterator_deref<Container>>>;

  Accumulator(Container&& container, AccumulateFunc accumulate_func)
      : container_(std::forward<Container>(container)),
        accumulate_func_(accumulate_func) {}

 public:
  Accumulator(Accumulator&&) = default;

  template <typename ContainerT>
  class Iterator {
   private:
    template <typename>
    friend class Iterator;
    IteratorWrapper<ContainerT> sub_iter_;
    IteratorWrapper<ContainerT> sub_end_;
    AccumulateFunc* accumulate_func_;
    std::optional<AccumVal> acc_val_;

   public:
    using iterator_category = std::input_iterator_tag;
    using value_type = AccumVal;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    Iterator(IteratorWrapper<ContainerT>&& sub_iter,
        IteratorWrapper<ContainerT>&& sub_end, AccumulateFunc& accumulate_fun)
        : sub_iter_{std::move(sub_iter)},
          sub_end_{std::move(sub_end)},
          accumulate_func_(&accumulate_fun),
          // only get first value if not an end iterator
          acc_val_{!(sub_iter_ != sub_end_)
                       ? std::nullopt
                       : std::make_optional<AccumVal>(*sub_iter_)} {}

    const AccumVal& operator*() const {
      return *acc_val_;
    }

    const AccumVal* operator->() const {
      return &*acc_val_;
    }

    Iterator& operator++() {
      ++sub_iter_;
      if (sub_iter_ != sub_end_) {
        *acc_val_ = std::invoke(*accumulate_func_, *acc_val_, *sub_iter_);
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
      return sub_iter_ != other.sub_iter_;
    }

    template <typename T>
    bool operator==(const Iterator<T>& other) const {
      return !(*this != other);
    }
  };

  Iterator<Container> begin() {
    return {get_begin(container_), get_end(container_), accumulate_func_};
  }

  Iterator<Container> end() {
    return {get_end(container_), get_end(container_), accumulate_func_};
  }
  Iterator<AsConst<Container>> begin() const {
    return {get_begin(std::as_const(container_)),
        get_end(std::as_const(container_)), accumulate_func_};
  }

  Iterator<AsConst<Container>> end() const {
    return {get_end(std::as_const(container_)),
        get_end(std::as_const(container_)), accumulate_func_};
  }
};

#endif
