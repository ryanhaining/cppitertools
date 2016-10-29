#ifndef ITERTOOLS_BASE_ITERATOR_HPP_
#define ITERTOOLS_BASE_ITERATOR_HPP_

#include "iterbase.hpp"
#include <cassert>

namespace iter {
namespace impl {
// iterator_end_type<C> is the type of C's end iterator
template <typename Container>
using iterator_end_type = decltype(std::end(std::declval<Container&>()));

template <typename Container>
class BaseIteratorImpl;


// If begin and end return the same type, type will be iterator_type<Container>
// If begin and end return different types, type will be BaseIteratorImpl
template <typename Container, bool same_types>
struct BaseIteratorImplType;

template <typename Container>
struct BaseIteratorImplType<Container, true>
: type_is<iterator_type<Container>>{};

template <typename Container>
struct BaseIteratorImplType<Container, false>
: type_is<BaseIteratorImpl<Container>>{};

template <typename Container>
using BaseIterator = typename BaseIteratorImplType<Container,
      std::is_same<impl::iterator_type<Container>,
                   impl::iterator_end_type<Container>>{}>::type;
}
}


template <typename Container>
class iter::impl::BaseIteratorImpl {
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

    BaseIteratorImpl& operator++() {
      assert(state_ == IterState::Normal); // because ++ing the end is UB
      ++sub_iter_;
      return *this;
    }

    // TODO implement const deref?
    decltype(auto) operator*() {
      assert(state_ == IterState::Normal); //because *ing the end is UB
      return *sub_iter_;
    }

    bool operator!=(const BaseIteratorImpl& other) const {
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
