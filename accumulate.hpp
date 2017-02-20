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
  Container container;
  AccumulateFunc accumulate_func;

  friend AccumulateFn;

  using AccumVal = std::remove_reference_t<std::result_of_t<AccumulateFunc(
      iterator_deref<Container>, iterator_deref<Container>)>>;

  Accumulator(Container&& in_container, AccumulateFunc in_accumulate_func)
      : container(std::forward<Container>(in_container)),
        accumulate_func(in_accumulate_func) {}

 public:
  Accumulator(Accumulator&&) = default;

  class Iterator : public std::iterator<std::input_iterator_tag, AccumVal> {
   private:
    IteratorWrapper<Container> sub_iter;
    IteratorWrapper<Container> sub_end;
    AccumulateFunc* accumulate_func;
    std::unique_ptr<AccumVal> acc_val;

   public:
    Iterator(IteratorWrapper<Container>&& iter,
        IteratorWrapper<Container>&& end, AccumulateFunc& in_accumulate_fun)
        : sub_iter{std::move(iter)},
          sub_end{std::move(end)},
          accumulate_func(&in_accumulate_fun),
          // only get first value if not an end iterator
          acc_val{!(iter != end) ? nullptr : new AccumVal(*iter)} {}

    Iterator(const Iterator& other)
        : sub_iter{other.sub_iter},
          sub_end{other.sub_end},
          accumulate_func{other.accumulate_func},
          acc_val{other.acc_val ? new AccumVal(*other.acc_val) : nullptr} {}

    Iterator& operator=(const Iterator& other) {
      if (this == &other) {
        return *this;
      }
      this->sub_iter = other.sub_iter;
      this->sub_end = other.sub_end;
      this->accumulate_func = other.accumulate_func;
      this->acc_val.reset(
          other.acc_val ? new AccumVal(*other.acc_val) : nullptr);
      return *this;
    }

    Iterator(Iterator&&) = default;
    Iterator& operator=(Iterator&&) = default;

    const AccumVal& operator*() const {
      return *this->acc_val;
    }

    const AccumVal* operator->() const {
      return this->acc_val.get();
    }

    Iterator& operator++() {
      ++this->sub_iter;
      if (this->sub_iter != this->sub_end) {
        *this->acc_val = (*accumulate_func)(*this->acc_val, *this->sub_iter);
      }
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    bool operator!=(const Iterator& other) const {
      return this->sub_iter != other.sub_iter;
    }

    bool operator==(const Iterator& other) const {
      return !(*this != other);
    }
  };

  Iterator begin() {
    return {std::begin(this->container), std::end(this->container),
        this->accumulate_func};
  }

  Iterator end() {
    return {std::end(this->container), std::end(this->container),
        this->accumulate_func};
  }
};

#endif
