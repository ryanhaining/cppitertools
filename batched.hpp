#ifndef ITER_BATCHED_HPP_
#define ITER_BATCHED_HPP_

#include "internal/iterator_wrapper.hpp"
#include "internal/iteratoriterator.hpp"
#include "internal/iterbase.hpp"

#include <algorithm>
#include <functional>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace iter {
  namespace impl {
    template <typename Container>
    class Batcher;

    using BatchedFn = IterToolFnBindSizeTSecond<Batcher>;
  }
  constexpr impl::BatchedFn batched{};
}

template <typename Container>
class iter::impl::Batcher {
 private:
  Container container_;
  std::size_t num_batches_;

  Batcher(Container&& container, std::size_t const num_batches)
      : container_(std::forward<Container>(container)), num_batches_{num_batches} {}

  friend BatchedFn;

  template <typename T>
  using IndexVector = std::vector<IteratorWrapper<T>>;
  template <typename T>
  using DerefVec = IterIterWrapper<IndexVector<T>>;

 public:
  Batcher(Batcher&&) = default;
  template <typename ContainerT>
  class Iterator {
   private:
    template <typename>
    friend class Iterator;
    std::shared_ptr<DerefVec<ContainerT>> batch_ =
        std::make_shared<DerefVec<ContainerT>>();
    IteratorWrapper<ContainerT> sub_iter_;
    IteratorWrapper<ContainerT> sub_end_;
    std::size_t num_batches_;
    std::size_t size_;
    std::size_t count_;

    bool done() const {
      return batch_->empty();
    }

    void refill_batch() {
      batch_->get().clear();
      if (count_ < num_batches_) {
        std::size_t const batch_size(size_ / num_batches_ + std::min<std::size_t>(1, (size_ % num_batches_) / (count_ + 1)));
        batch_->get().reserve(batch_size);
        for (std::size_t i = 0; i < batch_size; ++i) {
          batch_->get().push_back(sub_iter_);
          ++sub_iter_;
        }
        ++count_;
      }
    }

   public:
    using iterator_category = std::input_iterator_tag;
    using value_type = DerefVec<ContainerT>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    template <typename Iter1, typename Iter2, typename = void>
    struct distance_helper {
      static constexpr difference_type distance(Iter1 it1, Iter2 it2) {
        difference_type dist(0);
        for (; it1 != it2; ++it1)
          ++dist;
        return dist;
      } 
    };

    template <typename Iter1, typename Iter2>
    struct distance_helper<Iter1, Iter2, typename std::enable_if_t<std::is_arithmetic_v<typename Iter1::difference_type> && std::is_arithmetic_v<typename Iter2::difference_type>>> {
      static constexpr difference_type distance(Iter1 it1, Iter2 it2) {
        return std::distance(it1, it2);
      }
    };

    template <typename Iter1, typename Iter2>
    difference_type distance(Iter1 it1, Iter2 it2) const {
      return distance_helper<Iter1, Iter2>::distance(it1, it2);
    }

    Iterator(IteratorWrapper<ContainerT>&& sub_iter,
        IteratorWrapper<ContainerT>&& sub_end, std::size_t num_batches)
        : sub_iter_{std::move(sub_iter)},
          sub_end_{std::move(sub_end)},
          num_batches_{num_batches},
          size_{static_cast<std::size_t>(distance(sub_iter_, sub_end_))},
          count_{0} {
      refill_batch();
    }

    Iterator& operator++() {
      refill_batch();
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
      return *batch_;
    }

    DerefVec<ContainerT>* operator->() {
      return batch_.get();
    }
  };

  Iterator<Container> begin() {
    return {get_begin(container_), get_end(container_), num_batches_};
  }

  Iterator<Container> end() {
    return {get_end(container_), get_end(container_), num_batches_};
  }

  Iterator<AsConst<Container>> begin() const {
    return {get_begin(std::as_const(container_)),
        get_end(std::as_const(container_)), num_batches_};
  }

  Iterator<AsConst<Container>> end() const {
    return {get_end(std::as_const(container_)),
        get_end(std::as_const(container_)), num_batches_};
  }
};

#endif
