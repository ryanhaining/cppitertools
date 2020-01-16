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

  Compressed(Container&& in_container, Selector&& in_selectors)
      : container_(std::forward<Container>(in_container)),
        selectors_(std::forward<Selector>(in_selectors)) {}

 public:
  Compressed(Compressed&&) = default;
  template <typename ContainerT, typename SelectorT>
  class Iterator {
   private:
    template <typename, typename>
    friend class Iterator;
    IteratorWrapper<ContainerT> sub_iter_;
    IteratorWrapper<ContainerT> sub_end_;

    IteratorWrapper<SelectorT> selector_iter_;
    IteratorWrapper<SelectorT> selector_end_;

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
    using iterator_category = std::input_iterator_tag;
    using value_type = iterator_traits_deref<ContainerT>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    Iterator(IteratorWrapper<ContainerT>&& cont_iter,
        IteratorWrapper<ContainerT>&& cont_end,
        IteratorWrapper<SelectorT>&& sel_iter,
        IteratorWrapper<SelectorT>&& sel_end)
        : sub_iter_{std::move(cont_iter)},
          sub_end_{std::move(cont_end)},
          selector_iter_{std::move(sel_iter)},
          selector_end_{std::move(sel_end)} {
      skip_failures();
    }

    iterator_deref<ContainerT> operator*() {
      return *sub_iter_;
    }

    iterator_arrow<ContainerT> operator->() {
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

    template <typename T, typename U>
    bool operator!=(const Iterator<T, U>& other) const {
      return sub_iter_ != other.sub_iter_
             && selector_iter_ != other.selector_iter_;
    }

    template <typename T, typename U>
    bool operator==(const Iterator<T, U>& other) const {
      return !(*this != other);
    }
  };

  Iterator<Container, Selector> begin() {
    return {get_begin(container_), get_end(container_), get_begin(selectors_),
        get_end(selectors_)};
  }

  Iterator<Container, Selector> end() {
    return {get_end(container_), get_end(container_), get_end(selectors_),
        get_end(selectors_)};
  }

  Iterator<AsConst<Container>, AsConst<Selector>> begin() const {
    return {get_begin(std::as_const(container_)),
        get_end(std::as_const(container_)),
        get_begin(std::as_const(selectors_)),
        get_end(std::as_const(selectors_))};
  }

  Iterator<AsConst<Container>, AsConst<Selector>> end() const {
    return {get_end(std::as_const(container_)),
        get_end(std::as_const(container_)),
        get_end(std::as_const(selectors_)),
        get_end(std::as_const(selectors_))};
  }
};

template <typename Container, typename Selector>
iter::impl::Compressed<Container, Selector> iter::compress(
    Container&& container_, Selector&& selectors_) {
  return {
      std::forward<Container>(container_), std::forward<Selector>(selectors_)};
}

#endif
