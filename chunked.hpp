#ifndef ITER_CHUNKED_HPP_
#define ITER_CHUNKED_HPP_

#include "internal/iterator_wrapper.hpp"
#include "internal/iteratoriterator.hpp"
#include "internal/iterbase.hpp"

#include <algorithm>
#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

namespace iter {
  namespace impl {
    template <typename Container>
    class Chunker;

    using ChunkedFn = IterToolFnBindSizeTSecond<Chunker>;
  }
  constexpr impl::ChunkedFn chunked{};
}

template <typename Container>
class iter::impl::Chunker {
 private:
  Container container_;
  std::size_t chunk_size_;

  Chunker(Container&& container, std::size_t sz)
      : container_(std::forward<Container>(container)), chunk_size_{sz} {}

  friend ChunkedFn;

  template <typename T>
  using IndexVector = std::vector<IteratorWrapper<T>>;
  template <typename T>
  using DerefVec = IterIterWrapper<IndexVector<T>>;

 public:
  Chunker(Chunker&&) = default;
  template <typename ContainerT>
  class Iterator {
   private:
    template <typename>
    friend class Iterator;
    IteratorWrapper<ContainerT> sub_iter_;
    IteratorWrapper<ContainerT> sub_end_;
    DerefVec<ContainerT> chunk_;
    std::size_t chunk_size_ = 0;

    bool done() const {
      return chunk_.empty();
    }

    void refill_chunk() {
      chunk_.get().clear();
      std::size_t i{0};
      while (i < chunk_size_ && sub_iter_ != sub_end_) {
        chunk_.get().push_back(sub_iter_);
        ++sub_iter_;
        ++i;
      }
    }

   public:
    using iterator_category = std::input_iterator_tag;
    using value_type = DerefVec<ContainerT>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    Iterator(IteratorWrapper<ContainerT>&& sub_iter,
        IteratorWrapper<ContainerT>&& sub_end, std::size_t s)
        : sub_iter_{std::move(sub_iter)},
          sub_end_{std::move(sub_end)},
          chunk_size_{s} {
      chunk_.get().reserve(chunk_size_);
      refill_chunk();
    }

    Iterator& operator++() {
      refill_chunk();
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    template <typename T>
    bool operator!=(const Iterator<T>& other) const {
      return !(*this == other);
    }

    template <typename T>
    bool operator==(const Iterator<T>& other) const {
      return done() == other.done()
             && (done() || !(sub_iter_ != other.sub_iter_));
    }

    DerefVec<ContainerT>& operator*() {
      return chunk_;
    }

    DerefVec<ContainerT>* operator->() {
      return &chunk_;
    }
  };

  Iterator<Container> begin() {
    return {get_begin(container_), get_end(container_), chunk_size_};
  }

  Iterator<Container> end() {
    return {get_end(container_), get_end(container_), chunk_size_};
  }

  Iterator<AsConst<Container>> begin() const {
    return {get_begin(std::as_const(container_)),
        get_end(std::as_const(container_)), chunk_size_};
  }

  Iterator<AsConst<Container>> end() const {
    return {get_end(std::as_const(container_)),
        get_end(std::as_const(container_)), chunk_size_};
  }
};

#endif
