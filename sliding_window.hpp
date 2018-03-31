#ifndef ITER_SLIDING_WINDOW_HPP_
#define ITER_SLIDING_WINDOW_HPP_

#include "internal/iterator_wrapper.hpp"
#include "internal/iteratoriterator.hpp"
#include "internal/iterbase.hpp"

#include <deque>
#include <iterator>
#include <utility>

namespace iter {
  namespace impl {
    template <typename Container>
    class WindowSlider;
    using SlidingWindowFn = IterToolFnBindSizeTSecond<WindowSlider>;
  }
  constexpr impl::SlidingWindowFn sliding_window{};
}

template <typename Container>
class iter::impl::WindowSlider {
 private:
  Container container_;
  std::size_t window_size_;

  friend SlidingWindowFn;

  WindowSlider(Container&& container, std::size_t win_sz)
      : container_(std::forward<Container>(container)), window_size_{win_sz} {}

  template <typename T>
  using IndexVector = std::deque<IteratorWrapper<T>>;
  template <typename T>
  using DerefVec = IterIterWrapper<IndexVector<T>>;

 public:
  WindowSlider(WindowSlider&&) = default;
  template <typename ContainerT>
  class Iterator {
   private:
    template <typename>
    friend class Iterator;
    IteratorWrapper<ContainerT> sub_iter_;
    DerefVec<ContainerT> window_;

   public:
    using iterator_category = std::input_iterator_tag;
    using value_type = DerefVec<ContainerT>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    Iterator(IteratorWrapper<ContainerT>&& sub_iter,
        IteratorWrapper<ContainerT>&& sub_end, std::size_t window_sz)
        : sub_iter_(std::move(sub_iter)) {
      std::size_t i{0};
      while (i < window_sz && sub_iter_ != sub_end) {
        window_.get().push_back(sub_iter_);
        ++i;
        if (i != window_sz) {
          ++sub_iter_;
        }
      }
    }

    template <typename T>
    bool operator!=(const Iterator<T>& other) const {
      return sub_iter_ != other.sub_iter_;
    }

    template <typename T>
    bool operator==(const Iterator<T>& other) const {
      return !(*this != other);
    }

    DerefVec<ContainerT>& operator*() {
      return window_;
    }

    DerefVec<ContainerT>* operator->() {
      return window_;
    }

    Iterator& operator++() {
      ++sub_iter_;
      window_.get().pop_front();
      window_.get().push_back(sub_iter_);
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }
  };

  Iterator<Container> begin() {
    return {
        (window_size_ != 0 ? IteratorWrapper<Container>{get_begin(container_)}
                           : IteratorWrapper<Container>{get_end(container_)}),
        get_end(container_), window_size_};
  }

  Iterator<Container> end() {
    return {get_end(container_), get_end(container_), window_size_};
  }

  Iterator<AsConst<Container>> begin() const {
    return {(window_size_ != 0 ? IteratorWrapper<AsConst<Container>>{get_begin(
                                     std::as_const(container_))}
                               : IteratorWrapper<AsConst<Container>>{get_end(
                                     std::as_const(container_))}),
        get_end(std::as_const(container_)), window_size_};
  }

  Iterator<AsConst<Container>> end() const {
    return {get_end(std::as_const(container_)),
        get_end(std::as_const(container_)), window_size_};
  }
};

#endif
