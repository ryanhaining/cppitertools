#ifndef ITER_CHUNKED_HPP_
#define ITER_CHUNKED_HPP_

#include "internal/iterbase.hpp"
#include "internal/iteratoriterator.hpp"

#include <vector>
#include <algorithm>
#include <type_traits>
#include <functional>
#include <utility>
#include <iterator>
#include <initializer_list>

namespace iter {
  namespace impl {
    template <typename Container>
    class Chunker;
  }

  template <typename Container>
  impl::Chunker<Container> chunked(Container&&, std::size_t);

  template <typename T>
  impl::Chunker<std::initializer_list<T>> chunked(
      std::initializer_list<T>, std::size_t);
}

template <typename Container>
class iter::impl::Chunker {
 private:
  Container container;
  std::size_t chunk_size;

  Chunker(Container&& c, std::size_t sz)
      : container(std::forward<Container>(c)), chunk_size{sz} {}

  friend Chunker iter::chunked<Container>(Container&&, std::size_t);
  template <typename T>
  friend Chunker<std::initializer_list<T>> iter::chunked(
      std::initializer_list<T>, std::size_t);

  using IndexVector = std::vector<iterator_type<Container>>;
  using DerefVec = IterIterWrapper<IndexVector>;

 public:
  Chunker(Chunker&&) = default;
  class Iterator : public std::iterator<std::input_iterator_tag, DerefVec> {
   private:
    iterator_type<Container> sub_iter;
    iterator_type<Container> sub_end;
    DerefVec chunk;
    std::size_t chunk_size = 0;

    bool done() const {
      return this->chunk.empty();
    }

    void refill_chunk() {
      this->chunk.get().clear();
      std::size_t i{0};
      while (i < chunk_size && this->sub_iter != this->sub_end) {
        chunk.get().push_back(this->sub_iter);
        ++this->sub_iter;
        ++i;
      }
    }

   public:
    Iterator(iterator_type<Container>&& in_iter,
        iterator_type<Container>&& in_end, std::size_t s)
        : sub_iter{std::move(in_iter)},
          sub_end{std::move(in_end)},
          chunk_size{s} {
      this->chunk.get().reserve(this->chunk_size);
      this->refill_chunk();
    }

    Iterator& operator++() {
      this->refill_chunk();
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    bool operator!=(const Iterator& other) const {
      return !(*this == other);
    }

    bool operator==(const Iterator& other) const {
      return this->done() == other.done()
             && (this->done() || !(this->sub_iter != other.sub_iter));
    }

    DerefVec& operator*() {
      return this->chunk;
    }

    DerefVec* operator->() {
      return &this->chunk;
    }
  };

  Iterator begin() {
    return {std::begin(this->container), std::end(this->container), chunk_size};
  }

  Iterator end() {
    return {std::end(this->container), std::end(this->container), chunk_size};
  }
};

template <typename Container>
iter::impl::Chunker<Container> iter::chunked(
    Container&& container, std::size_t chunk_size) {
  return {std::forward<Container>(container), chunk_size};
}

template <typename T>
iter::impl::Chunker<std::initializer_list<T>> iter::chunked(
    std::initializer_list<T> il, std::size_t chunk_size) {
  return {std::move(il), chunk_size};
}

#endif
