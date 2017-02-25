#ifndef ITER_COMPRESS_H_
#define ITER_COMPRESS_H_

#include "internal/iterator_wrapper.hpp"
#include "internal/iterbase.hpp"

#include <iterator>
#include <utility>

namespace iter {
  namespace impl {
    template <typename Container, typename Selector>
    class Compressed;
  }

  template <typename Container, typename Selector>
  impl::Compressed<Container, Selector> compress(Container&&, Selector&&);
}

template <typename Container, typename Selector>
class iter::impl::Compressed {
 private:
  Container container_;
  Selector selectors_;

  friend Compressed iter::compress<Container, Selector>(
      Container&&, Selector&&);

  // Selector::Iterator type
  using selector_iter_type = decltype(get_begin(selectors_));

  Compressed(Container&& in_container, Selector&& in_selectors)
      : container_(std::forward<Container>(in_container)),
        selectors_(std::forward<Selector>(in_selectors)) {}

 public:
  Compressed(Compressed&&) = default;
  class Iterator : public std::iterator<std::input_iterator_tag,
                       iterator_traits_deref<Container>> {
   private:
    IteratorWrapper<Container> sub_iter_;
    IteratorWrapper<Container> sub_end_;

    selector_iter_type selector_iter_;
    selector_iter_type selector_end_;

    void increment_iterators() {
      ++sub_iter_;
      ++selector_iter_;
    }

    void skip_failures() {
      while (sub_iter_ != sub_end_ && selector_iter_ != selector_end_
             && !*selector_iter_) {
        increment_iterators();
      }
    }

   public:
    Iterator(IteratorWrapper<Container>&& cont_iter,
        IteratorWrapper<Container>&& cont_end, selector_iter_type&& sel_iter,
        selector_iter_type&& sel_end)
        : sub_iter_{std::move(cont_iter)},
          sub_end_{std::move(cont_end)},
          selector_iter_{std::move(sel_iter)},
          selector_end_{std::move(sel_end)} {
      skip_failures();
    }

    iterator_deref<Container> operator*() {
      return *sub_iter_;
    }

    iterator_arrow<Container> operator->() {
      return apply_arrow(sub_iter_);
    }

    Iterator& operator++() {
      increment_iterators();
      skip_failures();
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    bool operator!=(const Iterator& other) const {
      return sub_iter_ != other.sub_iter_
             && selector_iter_ != other.selector_iter_;
    }

    bool operator==(const Iterator& other) const {
      return !(*this != other);
    }
  };

  Iterator begin() {
    return {get_begin(container_), get_end(container_), get_begin(selectors_),
        get_end(selectors_)};
  }

  Iterator end() {
    return {get_end(container_), get_end(container_), get_end(selectors_),
        get_end(selectors_)};
  }
};

template <typename Container, typename Selector>
iter::impl::Compressed<Container, Selector> iter::compress(
    Container&& container_, Selector&& selectors_) {
  return {
      std::forward<Container>(container_), std::forward<Selector>(selectors_)};
}

#endif
