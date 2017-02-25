#ifndef ITER_ENUMERATE_H_
#define ITER_ENUMERATE_H_

#include "internal/iterator_wrapper.hpp"
#include "internal/iterbase.hpp"

#include <functional>
#include <initializer_list>
#include <iterator>
#include <type_traits>
#include <utility>

namespace iter {
  namespace impl {
    template <typename Container, typename Index>
    class Enumerable;

    using EnumerateFn = IterToolFnOptionalBindSecond<Enumerable, std::size_t>;
  }
  constexpr impl::EnumerateFn enumerate{};
}

template <typename Container, typename Index>
class iter::impl::Enumerable {
 private:
  Container container_;
  const Index start_;

  friend EnumerateFn;

  // for IterYield
  using BasePair = std::pair<Index, iterator_deref<Container>>;

  // Value constructor for use only in the enumerate function
  Enumerable(Container&& container, Index start)
      : container_(std::forward<Container>(container)), start_{start} {}

 public:
  Enumerable(Enumerable&&) = default;

  // "yielded" by the Enumerable::Iterator.  Has a .index, and a
  // .element referencing the value yielded by the subiterator
  class IterYield : public BasePair {
   public:
    using BasePair::BasePair;
    typename BasePair::first_type& index = BasePair::first;
    typename BasePair::second_type& element = BasePair::second;
  };

  //  Holds an iterator of the contained type and an Index for the
  //  index_.  Each call to ++ increments both of these data members.
  //  Each dereference returns an IterYield.
  class Iterator : public std::iterator<std::input_iterator_tag, IterYield> {
   private:
    IteratorWrapper<Container> sub_iter_;
    Index index_;

   public:
    Iterator(IteratorWrapper<Container>&& sub_iter, Index start)
        : sub_iter_{std::move(sub_iter)}, index_{start} {}

    IterYield operator*() {
      return {index_, *sub_iter_};
    }

    ArrowProxy<IterYield> operator->() {
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

    bool operator!=(const Iterator& other) const {
      return sub_iter_ != other.sub_iter_;
    }

    bool operator==(const Iterator& other) const {
      return !(*this != other);
    }
  };

  Iterator begin() {
    return {get_begin(container_), start_};
  }

  Iterator end() {
    return {get_end(container_), start_};
  }
};

#endif
