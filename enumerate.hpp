#ifndef ITER_ENUMERATE_H_
#define ITER_ENUMERATE_H_

#include "internal/iterbase.hpp"
#include "internal/iterator_wrapper.hpp"

#include <utility>
#include <iterator>
#include <functional>
#include <initializer_list>
#include <type_traits>

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
  Container container;
  const Index start;

  friend EnumerateFn;

  // for IterYield
  using BasePair = std::pair<Index, iterator_deref<Container>>;

  // Value constructor for use only in the enumerate function
  Enumerable(Container&& in_container, Index in_start)
      : container(std::forward<Container>(in_container)), start{in_start} {}

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
  //  index.  Each call to ++ increments both of these data members.
  //  Each dereference returns an IterYield.
  class Iterator : public std::iterator<std::input_iterator_tag, IterYield> {
   private:
    IteratorWrapper<Container> sub_iter;
    Index index;

   public:
    Iterator(IteratorWrapper<Container>&& si, Index start)
        : sub_iter{std::move(si)}, index{start} {}

    IterYield operator*() {
      return {this->index, *this->sub_iter};
    }

    ArrowProxy<IterYield> operator->() {
      return {**this};
    }

    Iterator& operator++() {
      ++this->sub_iter;
      ++this->index;
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
    return {std::begin(this->container), start};
  }

  Iterator end() {
    return {std::end(this->container), start};
  }
};

#endif
