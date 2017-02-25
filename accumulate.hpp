#ifndef ITER_ACCUMULATE_H_
#define ITER_ACCUMULATE_H_

#include "internal/iterator_wrapper.hpp"
#include "internal/iterbase.hpp"

#include <functional>
#include <iterator>
#include <memory>
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
  AccumulateFunc accumulate_func_;

  friend AccumulateFn;

  using AccumVal = std::remove_reference_t<std::result_of_t<AccumulateFunc(
      iterator_deref<Container>, iterator_deref<Container>)>>;

  Accumulator(Container&& container, AccumulateFunc accumulate_func)
      : container_(std::forward<Container>(container)),
        accumulate_func_(accumulate_func) {}

 public:
  Accumulator(Accumulator&&) = default;

  class Iterator : public std::iterator<std::input_iterator_tag, AccumVal> {
   private:
    IteratorWrapper<Container> sub_iter_;
    IteratorWrapper<Container> sub_end_;
    AccumulateFunc* accumulate_func_;
    std::unique_ptr<AccumVal> acc_val_;

   public:
    Iterator(IteratorWrapper<Container>&& sub_iter,
        IteratorWrapper<Container>&& sub_end, AccumulateFunc& accumulate_fun)
        : sub_iter_{std::move(sub_iter)},
          sub_end_{std::move(sub_end)},
          accumulate_func_(&accumulate_fun),
          // only get first value if not an end iterator
          acc_val_{
              !(sub_iter_ != sub_end_) ? nullptr : new AccumVal(*sub_iter_)} {}

    Iterator(const Iterator& other)
        : sub_iter_{other.sub_iter_},
          sub_end_{other.sub_end_},
          accumulate_func_{other.accumulate_func_},
          acc_val_{other.acc_val_ ? new AccumVal(*other.acc_val_) : nullptr} {}

    Iterator& operator=(const Iterator& other) {
      if (this == &other) {
        return *this;
      }
      sub_iter_ = other.sub_iter_;
      sub_end_ = other.sub_end_;
      accumulate_func_ = other.accumulate_func_;
      acc_val_.reset(other.acc_val_ ? new AccumVal(*other.acc_val_) : nullptr);
      return *this;
    }

    Iterator(Iterator&&) = default;
    Iterator& operator=(Iterator&&) = default;

    const AccumVal& operator*() const {
      return *acc_val_;
    }

    const AccumVal* operator->() const {
      return acc_val_.get();
    }

    Iterator& operator++() {
      ++sub_iter_;
      if (sub_iter_ != sub_end_) {
        *acc_val_ = (*accumulate_func_)(*acc_val_, *sub_iter_);
      }
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    bool operator!=(const Iterator& other) const {
      return sub_iter_ != other.sub_iter_;
    }

    bool operator==(const Iterator& other) const {
      return !(*this != other);
    }
  };

  Iterator begin() {
    return {get_begin(container_), get_end(container_), accumulate_func_};
  }

  Iterator end() {
    return {get_end(container_), get_end(container_), accumulate_func_};
  }
};

#endif
