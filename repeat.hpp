#ifndef ITER_REPEAT_HPP_
#define ITER_REPEAT_HPP_

#include <iterator>
#include <type_traits>
#include <utility>

namespace iter {
  namespace impl {
    template <typename T>
    class RepeaterWithCount;
  }

  template <typename T>
  constexpr impl::RepeaterWithCount<T> repeat(T&&, int);
}

template <typename T>
class iter::impl::RepeaterWithCount {
  // see stackoverflow.com/questions/32174186/ about why this isn't
  // declaring just a specialization as friend
  template <typename U>
  friend constexpr RepeaterWithCount<U> iter::repeat(U&&, int);

 private:
  T elem_;
  int count_;

  constexpr RepeaterWithCount(T e, int c)
      : elem_(std::forward<T>(e)), count_{c} {}

  using TPlain = typename std::remove_reference<T>::type;

 public:
  RepeaterWithCount(RepeaterWithCount&&) = default;

  class Iterator {
   private:
    const TPlain* elem_;
    int count_;

   public:
    using iterator_category = std::input_iterator_tag;
    using value_type = const TPlain;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    constexpr Iterator(const TPlain* e, int c) : elem_{e}, count_{c} {}

    Iterator& operator++() {
      --this->count_;
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    constexpr bool operator!=(const Iterator& other) const {
      return !(*this == other);
    }

    constexpr bool operator==(const Iterator& other) const {
      return this->count_ == other.count_;
    }

    constexpr const TPlain& operator*() const {
      return *this->elem_;
    }

    constexpr const TPlain* operator->() const {
      return this->elem_;
    }
  };

  constexpr Iterator begin() const {
    return {&this->elem_, this->count_};
  }

  constexpr Iterator end() const {
    return {&this->elem_, 0};
  }
};

template <typename T>
constexpr iter::impl::RepeaterWithCount<T> iter::repeat(T&& e, int count_) {
  return {std::forward<T>(e), count_ < 0 ? 0 : count_};
}

namespace iter {
  namespace impl {
    template <typename T>
    class Repeater;
  }

  template <typename T>
  constexpr impl::Repeater<T> repeat(T&&);
}

template <typename T>
class iter::impl::Repeater {
  template <typename U>
  friend constexpr Repeater<U> iter::repeat(U&&);

 private:
  using TPlain = typename std::remove_reference<T>::type;
  T elem_;

  constexpr Repeater(T e) : elem_(std::forward<T>(e)) {}

 public:
  Repeater(Repeater&&) = default;

  class Iterator {
   private:
    const TPlain* elem_;

   public:
    using iterator_category = std::input_iterator_tag;
    using value_type = const TPlain;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    constexpr Iterator(const TPlain* e) : elem_{e} {}

    constexpr const Iterator& operator++() const {
      return *this;
    }

    constexpr Iterator operator++(int)const {
      return *this;
    }

    constexpr bool operator!=(const Iterator&) const {
      return true;
    }

    constexpr bool operator==(const Iterator&) const {
      return false;
    }

    constexpr const TPlain& operator*() const {
      return *this->elem_;
    }

    constexpr const TPlain* operator->() const {
      return this->elem_;
    }
  };

  constexpr Iterator begin() const {
    return {&this->elem_};
  }

  constexpr Iterator end() const {
    return {nullptr};
  }
};

template <typename T>
constexpr iter::impl::Repeater<T> iter::repeat(T&& e) {
  return {std::forward<T>(e)};
}

#endif
