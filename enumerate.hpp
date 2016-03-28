#ifndef ITER_ENUMERATE_H_
#define ITER_ENUMERATE_H_

#include "internal/iterbase.hpp"

#include <utility>
#include <iterator>
#include <functional>
#include <initializer_list>
#include <type_traits>

namespace iter {
  namespace impl {
    template <typename Container>
    class Enumerable;
    struct EnumerateFn;
  }
}

template <typename Container>
class iter::impl::Enumerable {
 private:
  Container container;
  const std::size_t start;

  friend struct EnumerateFn;

  // for IterYield
  using BasePair = std::pair<std::size_t, iterator_deref<Container>>;

  // Value constructor for use only in the enumerate function
  Enumerable(Container&& in_container, std::size_t in_start)
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

  //  Holds an iterator of the contained type and a size_t for the
  //  index.  Each call to ++ increments both of these data members.
  //  Each dereference returns an IterYield.
  class Iterator : public std::iterator<std::input_iterator_tag, IterYield> {
   private:
    iterator_type<Container> sub_iter;
    std::size_t index;

   public:
    Iterator(iterator_type<Container>&& si, std::size_t start)
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

struct iter::impl::EnumerateFn {
  template <typename Container>
  Enumerable<Container> operator()(
      Container&& container, std::size_t start=0) const {
    return {std::forward<Container>(container), start};
  }

  template <typename T>
  Enumerable<std::initializer_list<T>> operator()(
      std::initializer_list<T> il, std::size_t start=0) const {
    return {std::move(il), start};
  }
};

namespace iter {
constexpr impl::EnumerateFn enumerate;
}

#endif
