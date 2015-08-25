#ifndef ITER_REPEAT_HPP_
#define ITER_REPEAT_HPP_

#include <iterator>
#include <type_traits>
#include <utility>

namespace iter {
  namespace impl {
    template <typename T>
    class RepeaterWithCount;

    // forward isn't constexpr until c++14
    template <typename T>
    constexpr T&& forward(typename std::remove_reference<T>::type& t) {
      return static_cast<T&&>(t);
    }

    template <typename T>
    constexpr T&& forward(typename std::remove_reference<T>::type&& t) {
      return static_cast<T&&>(t);
    }
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
  T elem;
  int count;

  constexpr RepeaterWithCount(T e, int c)
      : elem(impl::forward<T>(e)), count{c} {}

  using TPlain = typename std::remove_reference<T>::type;

 public:
  RepeaterWithCount(RepeaterWithCount&&) = default;

  class Iterator : public std::iterator<std::input_iterator_tag, const TPlain> {
   private:
    const TPlain* elem;
    int count;

   public:
    constexpr Iterator(const TPlain* e, int c) : elem{e}, count{c} {}

    Iterator& operator++() {
      --this->count;
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
      return this->count == other.count;
    }

    constexpr const TPlain& operator*() const {
      return *this->elem;
    }

    constexpr const TPlain* operator->() const {
      return this->elem;
    }
  };

  constexpr Iterator begin() const {
    return {&this->elem, this->count};
  }

  constexpr Iterator end() const {
    return {&this->elem, 0};
  }
};

template <typename T>
constexpr iter::impl::RepeaterWithCount<T> iter::repeat(T&& e, int count) {
  return {impl::forward<T>(e), count < 0 ? 0 : count};
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
  T elem;

  constexpr Repeater(T e) : elem(impl::forward<T>(e)) {}

 public:
  Repeater(Repeater&&) = default;

  class Iterator : public std::iterator<std::input_iterator_tag, const TPlain> {
   private:
    const TPlain* elem;

   public:
    constexpr Iterator(const TPlain* e) : elem{e} {}

    constexpr const Iterator& operator++() const {
      return *this;
    }

    constexpr Iterator operator++(int) const {
      return *this;
    }

    constexpr bool operator!=(const Iterator&) const {
      return true;
    }

    constexpr bool operator==(const Iterator&) const {
      return false;
    }

    constexpr const TPlain& operator*() const {
      return *this->elem;
    }

    constexpr const TPlain* operator->() const {
      return this->elem;
    }
  };

  constexpr Iterator begin() const {
    return {&this->elem};
  }

  constexpr Iterator end() const {
    return {nullptr};
  }
};

template <typename T>
constexpr iter::impl::Repeater<T> iter::repeat(T&& e) {
  return {impl::forward<T>(e)};
}

#endif
