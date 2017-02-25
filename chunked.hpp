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

  using IndexVector = std::vector<IteratorWrapper<Container>>;
  using DerefVec = IterIterWrapper<IndexVector>;

 public:
  Chunker(Chunker&&) = default;
  class Iterator : public std::iterator<std::input_iterator_tag, DerefVec> {
   private:
    IteratorWrapper<Container> sub_iter_;
    IteratorWrapper<Container> sub_end_;
    DerefVec chunk_;
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
    Iterator(IteratorWrapper<Container>&& sub_iter,
        IteratorWrapper<Container>&& sub_end, std::size_t s)
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

    bool operator!=(const Iterator& other) const {
      return !(*this == other);
    }

    bool operator==(const Iterator& other) const {
      return done() == other.done()
             && (done() || !(sub_iter_ != other.sub_iter_));
    }

    DerefVec& operator*() {
      return chunk_;
    }

    DerefVec* operator->() {
      return &chunk_;
    }
  };

  Iterator begin() {
    return {get_begin(container_), get_end(container_), chunk_size_};
  }

  Iterator end() {
    return {get_end(container_), get_end(container_), chunk_size_};
  }
};

#endif
