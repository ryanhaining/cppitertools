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

  using IndexVector = std::deque<IteratorWrapper<Container>>;
  using DerefVec = IterIterWrapper<IndexVector>;

 public:
  WindowSlider(WindowSlider&&) = default;
  class Iterator : public std::iterator<std::input_iterator_tag, DerefVec> {
   private:
    IteratorWrapper<Container> sub_iter_;
    DerefVec window_;

   public:
    Iterator(IteratorWrapper<Container>&& sub_iter,
        IteratorWrapper<Container>&& sub_end, std::size_t window_sz)
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

    bool operator!=(const Iterator& other) const {
      return sub_iter_ != other.sub_iter_;
    }

    bool operator==(const Iterator& other) const {
      return !(*this != other);
    }

    DerefVec& operator*() {
      return window_;
    }

    DerefVec* operator->() {
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

  Iterator begin() {
    return {
        (window_size_ != 0 ? IteratorWrapper<Container>{get_begin(container_)}
                           : IteratorWrapper<Container>{get_end(container_)}),
        get_end(container_), window_size_};
  }

  Iterator end() {
    return {get_end(container_), get_end(container_), window_size_};
  }
};

#endif
