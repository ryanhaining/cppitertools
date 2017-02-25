#ifndef ITER_DROPWHILE_H_
#define ITER_DROPWHILE_H_

#include "filter.hpp"
#include "internal/iterator_wrapper.hpp"
#include "internal/iterbase.hpp"

#include <iterator>
#include <utility>

namespace iter {
  namespace impl {
    template <typename FilterFunc, typename Container>
    class Dropper;

    using DropWhileFn = IterToolFnOptionalBindFirst<Dropper, BoolTester>;
  }
  constexpr impl::DropWhileFn dropwhile{};
}

template <typename FilterFunc, typename Container>
class iter::impl::Dropper {
 private:
  Container container_;
  FilterFunc filter_func_;

  friend DropWhileFn;

  Dropper(FilterFunc filter_func, Container&& container)
      : container_(std::forward<Container>(container)),
        filter_func_(filter_func) {}

 public:
  Dropper(Dropper&&) = default;
  class Iterator : public std::iterator<std::input_iterator_tag,
                       iterator_traits_deref<Container>> {
   private:
    using Holder = DerefHolder<iterator_deref<Container>>;
    IteratorWrapper<Container> sub_iter_;
    IteratorWrapper<Container> sub_end_;
    Holder item_;
    FilterFunc* filter_func_;

    void inc_sub_iter() {
      ++sub_iter_;
      if (sub_iter_ != sub_end_) {
        item_.reset(*sub_iter_);
      }
    }

    // skip all values for which the predicate is true
    void skip_passes() {
      while (sub_iter_ != sub_end_ && (*filter_func_)(item_.get())) {
        inc_sub_iter();
      }
    }

   public:
    Iterator(IteratorWrapper<Container>&& sub_iter,
        IteratorWrapper<Container>&& sub_end, FilterFunc& filter_func)
        : sub_iter_{std::move(sub_iter)},
          sub_end_{std::move(sub_end)},
          filter_func_(&filter_func) {
      if (sub_iter_ != sub_end_) {
        item_.reset(*sub_iter_);
      }
      skip_passes();
    }

    typename Holder::reference operator*() {
      return item_.get();
    }

    typename Holder::pointer operator->() {
      return item_.get_ptr();
    }

    Iterator& operator++() {
      inc_sub_iter();
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
    return {get_begin(container_), get_end(container_), filter_func_};
  }

  Iterator end() {
    return {get_end(container_), get_end(container_), filter_func_};
  }
};

#endif
