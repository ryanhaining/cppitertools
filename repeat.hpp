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
  friend RepeaterWithCount iter::repeat<T>(T&&, int);

 private:
  T elem;
  int count;

  constexpr RepeaterWithCount(T e, int c)
      : elem(std::forward<T>(e)), count{c} {}

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

  constexpr Iterator begin() {
    return {&this->elem, this->count};
  }

  constexpr Iterator end() {
    return {&this->elem, 0};
  }
};

template <typename T>
constexpr iter::impl::RepeaterWithCount<T> iter::repeat(T&& e, int count) {
  return {std::forward<T>(e), count < 0 ? 0 : count};
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
  friend Repeater iter::repeat<T>(T&&);

 private:
  using TPlain = typename std::remove_reference<T>::type;
  T elem;

  constexpr Repeater(T e) : elem(std::forward<T>(e)) {}

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

  constexpr Iterator begin() {
    return {&this->elem};
  }

  constexpr Iterator end() {
    return {nullptr};
  }
};

template <typename T>
constexpr iter::impl::Repeater<T> iter::repeat(T&& e) {
  return {std::forward<T>(e)};
}

#endif
