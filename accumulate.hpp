#ifndef ITER_ACCUMULATE_H_
#define ITER_ACCUMULATE_H_

#include "internal/iterbase.hpp"

#include <utility>
#include <iterator>
#include <initializer_list>
#include <functional>
#include <type_traits>
#include <memory>

namespace iter {
  namespace impl {
    template <typename Container, typename AccumulateFunc>
    class Accumulator;
  }

  template <typename Container, typename AccumulateFunc>
  impl::Accumulator<Container, AccumulateFunc> accumulate(
      Container&&, AccumulateFunc);

  template <typename T, typename AccumulateFunc>
  impl::Accumulator<std::initializer_list<T>, AccumulateFunc> accumulate(
      std::initializer_list<T>, AccumulateFunc);
}

template <typename Container, typename AccumulateFunc>
class iter::impl::Accumulator {
 private:
  Container container;
  AccumulateFunc accumulate_func;

  friend Accumulator iter::accumulate<Container, AccumulateFunc>(
      Container&&, AccumulateFunc);

  template <typename T, typename AF>
  friend Accumulator<std::initializer_list<T>, AF> iter::accumulate(
      std::initializer_list<T>, AF);

  // AccumVal must be default constructible
  using AccumVal =
      typename std::remove_reference<typename std::result_of<AccumulateFunc(
          iterator_deref<Container>, iterator_deref<Container>)>::type>::type;

  Accumulator(Container&& in_container, AccumulateFunc in_accumulate_func)
      : container(std::forward<Container>(in_container)),
        accumulate_func(in_accumulate_func) {}

 public:
  Accumulator(Accumulator&&) = default;

  class Iterator : public std::iterator<std::input_iterator_tag, AccumVal> {
   private:
    iterator_type<Container> sub_iter;
    iterator_type<Container> sub_end;
    AccumulateFunc* accumulate_func;
    std::unique_ptr<AccumVal> acc_val;

   public:
    Iterator(iterator_type<Container>&& iter, iterator_type<Container>&& end,
        AccumulateFunc& in_accumulate_fun)
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
      if (this == &other) { return *this; }
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

template <typename Container, typename AccumulateFunc>
iter::impl::Accumulator<Container, AccumulateFunc> iter::accumulate(
    Container&& container, AccumulateFunc accumulate_func) {
  return {std::forward<Container>(container), accumulate_func};
}

template <typename T, typename AccumulateFunc>
iter::impl::Accumulator<std::initializer_list<T>, AccumulateFunc>
iter::accumulate(std::initializer_list<T> il, AccumulateFunc accumulate_func) {
  return {std::move(il), accumulate_func};
}

namespace iter {
  template <typename Container>
  auto accumulate(Container&& container) -> decltype(accumulate(
      std::forward<Container>(container),
      std::plus<typename std::
              remove_reference<impl::iterator_deref<Container>>::type>{})) {
    return accumulate(
        std::forward<Container>(container),
        std::plus<typename std::
                remove_reference<impl::iterator_deref<Container>>::type>{});
  }

  template <typename T>
  auto accumulate(std::initializer_list<T> il)
      -> decltype(accumulate(std::move(il), std::plus<T>{})) {
    return accumulate(std::move(il), std::plus<T>{});
  }
}

#endif
