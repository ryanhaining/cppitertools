#ifndef ITER_ENUMERATE_H_
#define ITER_ENUMERATE_H_

#include "internal/iterator_wrapper.hpp"
#include "internal/iterbase.hpp"

#include <functional>
#include <initializer_list>
#include <iterator>
#include <tuple>
#include <type_traits>
#include <utility>

namespace iter {
  namespace impl {
    template <typename Index, typename Elem>
    using EnumBasePair = std::pair<Index, Elem>;

    // "yielded" by the Enumerable::Iterator.  Has a .index, and a
    // .element referencing the value yielded by the subiterator
    template <typename Index, typename Elem>
    class EnumIterYield : public EnumBasePair<Index, Elem> {
      using BasePair = EnumBasePair<Index, Elem>;
      using BasePair::BasePair;

     public:
      typename BasePair::first_type index = BasePair::first;
      typename BasePair::second_type element = BasePair::second;
    };

    template <typename Container, typename Index>
    class Enumerable;

    using EnumerateFn = IterToolFnOptionalBindSecond<Enumerable, std::size_t>;
  }
  constexpr impl::EnumerateFn enumerate{};
}

namespace std {
  template <typename Index, typename Elem>
  class tuple_size<iter::impl::EnumIterYield<Index, Elem>>
      : public tuple_size<iter::impl::EnumBasePair<Index, Elem>> {};

  template <std::size_t N, typename Index, typename Elem>
  class tuple_element<N, iter::impl::EnumIterYield<Index, Elem>>
      : public tuple_element<N, iter::impl::EnumBasePair<Index, Elem>> {};
}

template <typename Container, typename Index>
class iter::impl::Enumerable {
 private:
  Container container_;
  const Index start_;

  friend EnumerateFn;

  // Value constructor for use only in the enumerate function
  Enumerable(Container&& container, Index start)
      : container_(std::forward<Container>(container)), start_{start} {}

 public:
  Enumerable(Enumerable&&) = default;

  template <typename T>
  using IterYield = EnumIterYield<Index, iterator_deref<T>>;

  //  Holds an iterator of the contained type and an Index for the
  //  index_.  Each call to ++ increments both of these data members.
  //  Each dereference returns an IterYield.
  template <typename ContainerT>
  class Iterator {
   private:
    template <typename>
    friend class Iterator;
    IteratorWrapper<ContainerT> sub_iter_;
    Index index_;

   public:
    using iterator_category = std::input_iterator_tag;
    using value_type = IterYield<ContainerT>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    Iterator(IteratorWrapper<ContainerT>&& sub_iter, Index start)
        : sub_iter_{std::move(sub_iter)}, index_{start} {}

    IterYield<ContainerT> operator*() {
      return {index_, *sub_iter_};
    }

    ArrowProxy<IterYield<ContainerT>> operator->() {
      return {**this};
    }

    Iterator& operator++() {
      ++sub_iter_;
      ++index_;
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
    return {get_begin(container_), start_};
  }

  Iterator<Container> end() {
    return {get_end(container_), start_};
  }

  Iterator<AsConst<Container>> begin() const {
    return {get_begin(std::as_const(container_)), start_};
  }

  Iterator<AsConst<Container>> end() const {
    return {get_end(std::as_const(container_)), start_};
  }
};
#endif
