#ifndef ITERTOOLS_BASE_ITERATOR_HPP_
#define ITERTOOLS_BASE_ITERATOR_HPP_

#include "iterbase.hpp"
#include <cassert>

namespace iter {
namespace impl {
// iterator_end_type<C> is the type of C's end iterator
template <typename Container>
using iterator_end_type = decltype(std::end(std::declval<Container&>()));

template <typename Container, bool SameIteratorTypes>
class BaseIteratorImpl;

template <typename Container>
using BaseIterator = impl::BaseIteratorImpl<Container,
      std::is_same<impl::iterator_type<Container>,
                   impl::iterator_end_type<Container>>{}>;
}
}


// Container's begin() and end() are the same type.
template <typename Container>
class iter::impl::BaseIteratorImpl<Container, true> {
 private:
  static_assert(
      std::is_same<iterator_type<Container>, iterator_end_type<Container>>{},
      "");
  using SubIter = iterator_type<Container>;

  SubIter sub_iter_;
 public:
  static constexpr bool same_iterator_types = true;
  BaseIteratorImpl() = default;
  BaseIteratorImpl(SubIter&& it) : sub_iter_(std::move(it)) { }

  // I'm choosing to use named functions so that they aren't accidentally
  // used by subclasses
  bool not_equal(const BaseIteratorImpl& other) const {
    return sub_iter_ != other.sub_iter_;
  }

  void inc() {
    ++sub_iter_;
  }

  // TODO implement const deref?
  decltype(auto) deref() {
    return *sub_iter_;
  }
};

template <typename Container>
class iter::impl::BaseIteratorImpl<Container, false> {
  private:
    static_assert(
        !std::is_same<iterator_type<Container>, iterator_end_type<Container>>{},
        "");

    using SubIter = iterator_type<Container>;
    using SubEnd = iterator_end_type<Container>;

    enum class IterState { Normal, End, Uninitialized};

    void destroy_sub() {
      if (state_ == IterState::Normal) {
        sub_iter_.~SubIter();
      } else if (state_ == IterState::End) {
        sub_end_.~SubEnd();
      }
      state_ = IterState::Uninitialized;
    }

    template <typename T>
    void copy_or_move_sub_from(T&& other) {
      if (this == &other) { return; }
      if (state_ == other.state_) {
        if (state_ == IterState::Normal) {
          sub_iter_ = std::forward<T>(other).sub_iter_;
        } else if (state_ == IterState::End) {
          sub_end_ = std::forward<T>(other).sub_end_;
        }
      } else {
        // state_s are different, must destroy and reconstruct
        destroy_sub();
        if (other.state_ == IterState::Normal) {
          new (&sub_iter_) SubIter(std::forward<T>(other).sub_iter_);
        } else if (other.state_ == IterState::End) {
          new (&sub_end_) SubEnd(std::forward<T>(other).sub_end_);
        }
        state_ = other.state_;
      }
    }


    void copy_sub_from(const BaseIteratorImpl& other) {
      copy_or_move_sub_from(other);
    }

    void move_sub_from(BaseIteratorImpl&& other) {
      copy_or_move_sub_from(std::move(other));
    }

    // TODO replace with std::variant when C++17 is going strong
    union {
      SubIter sub_iter_;
      SubEnd sub_end_;
    };
    IterState state_{IterState::Uninitialized};

  public:
    BaseIteratorImpl() {}

    BaseIteratorImpl(const BaseIteratorImpl& other) {
      copy_sub_from(other);
    }

    BaseIteratorImpl& operator=(const BaseIteratorImpl& other) {
      copy_sub_from(other);
      return *this;
    }

    BaseIteratorImpl(BaseIteratorImpl&& other) {
      move_sub_from(std::move(other));
    }

    BaseIteratorImpl& operator=(BaseIteratorImpl&& other) {
      move_sub_from(std::move(other));
      return *this;
    }

    BaseIteratorImpl(SubIter&& it)
      : sub_iter_{std::move(it)},
      state_{IterState::Normal} { }

    BaseIteratorImpl(SubEnd&& it)
      : sub_end_(std::move(it)),
      state_{IterState::End} { }

    void inc() {
      assert(state_ == IterState::Normal); // because ++ing the end is UB
      ++sub_iter_;
    }

    // TODO implement const deref?
    decltype(auto) deref() {
      assert(state_ == IterState::Normal); //because *ing the end is UB
      return *sub_iter_;
    }

    bool not_equal(const BaseIteratorImpl& other) const {
      assert(state_ != IterState::Uninitialized
          && other.state_ != IterState::Uninitialized);
      if (state_ == other.state_) {
        if (state_ == IterState::End) {
          return sub_end_ != other.sub_end_;
        } else {
          return sub_iter_ != other.sub_iter_;
        }
      } else {
        if(state_ == IterState::Normal) { // other is End
          return sub_iter_ != other.sub_end_;
        } else { // other is Normal
          return sub_end_ != other.sub_iter_;
        }
      }
    }

    ~BaseIteratorImpl() {
      this->destroy_sub();
    }

};



#endif
