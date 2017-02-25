#ifndef ITERTOOLS_ITERATOR_WRAPPER_HPP_
#define ITERTOOLS_ITERATOR_WRAPPER_HPP_

#include <cassert>
#include "iterbase.hpp"

namespace iter {
  namespace impl {
    // iterator_end_type<C> is the type of C's end iterator
    template <typename Container>
    using iterator_end_type = decltype(get_end(std::declval<Container&>()));

    template <typename SubIter, typename SubEnd>
    class IteratorWrapperImpl;

    // If begin and end return the same type, type will be
    // iterator_type<Container>
    // If begin and end return different types, type will be IteratorWrapperImpl
    template <typename Container, bool same_types>
    struct IteratorWrapperImplType;

    template <typename Container>
    struct IteratorWrapperImplType<Container, true>
        : type_is<iterator_type<Container>> {};

    template <typename Container>
    struct IteratorWrapperImplType<Container, false>
        : type_is<IteratorWrapperImpl<iterator_type<Container>,
              iterator_end_type<Container>>> {};

    template <typename Container>
    using IteratorWrapper = typename IteratorWrapperImplType<Container,
        std::is_same<impl::iterator_type<Container>,
            impl::iterator_end_type<Container>>{}>::type;
  }
}

template <typename SubIter, typename SubEnd>
class iter::impl::IteratorWrapperImpl {
 private:
  static_assert(!std::is_same<SubIter, SubEnd>{}, "");
  enum class IterState { Normal, End, Uninitialized };

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
    if (this == &other) {
      return;
    }
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

  void copy_sub_from(const IteratorWrapperImpl& other) {
    copy_or_move_sub_from(other);
  }

  void move_sub_from(IteratorWrapperImpl&& other) {
    copy_or_move_sub_from(std::move(other));
  }

  // TODO replace with std::variant when C++17 is going strong
  union {
    SubIter sub_iter_;
    SubEnd sub_end_;
  };
  IterState state_{IterState::Uninitialized};

 public:
  IteratorWrapperImpl() : IteratorWrapperImpl(SubIter{}) {}

  IteratorWrapperImpl(const IteratorWrapperImpl& other) {
    copy_sub_from(other);
  }

  IteratorWrapperImpl& operator=(const IteratorWrapperImpl& other) {
    copy_sub_from(other);
    return *this;
  }

  IteratorWrapperImpl(IteratorWrapperImpl&& other) {
    move_sub_from(std::move(other));
  }

  IteratorWrapperImpl& operator=(IteratorWrapperImpl&& other) {
    move_sub_from(std::move(other));
    return *this;
  }

  IteratorWrapperImpl(SubIter&& it)
      : sub_iter_{std::move(it)}, state_{IterState::Normal} {}

  IteratorWrapperImpl(SubEnd&& it)
      : sub_end_(std::move(it)), state_{IterState::End} {}

  IteratorWrapperImpl& operator++() {
    assert(state_ == IterState::Normal);  // because ++ing the end is UB
    ++sub_iter_;
    return *this;
  }

  decltype(auto) operator*() {
    assert(state_ == IterState::Normal);  // because *ing the end is UB
    return *sub_iter_;
  }

  decltype(auto) operator*() const {
    assert(state_ == IterState::Normal);  // because *ing the end is UB
    return *sub_iter_;
  }

  decltype(auto) operator-> () {
    assert(state_ == IterState::Normal);
    return apply_arrow(sub_iter_);
  }

  decltype(auto) operator-> () const {
    assert(state_ == IterState::Normal);
    return apply_arrow(sub_iter_);
  }

  bool operator!=(const IteratorWrapperImpl& other) const {
    assert(state_ != IterState::Uninitialized
           && other.state_ != IterState::Uninitialized);
    if (state_ == other.state_) {
      if (state_ == IterState::End) {
        // NOTE this used to be return sub_end_ != other.sub_end_;
        // but rangev3 sentinels aren't comparable
        // https://github.com/ericniebler/range-v3/issues/564
        return false;
      } else {
        return sub_iter_ != other.sub_iter_;
      }
    } else {
      if (state_ == IterState::Normal) {  // other is End
        return sub_iter_ != other.sub_end_;
      } else {  // other is Normal
        return sub_end_ != other.sub_iter_;
      }
    }
  }

  ~IteratorWrapperImpl() {
    this->destroy_sub();
  }
};

#endif
