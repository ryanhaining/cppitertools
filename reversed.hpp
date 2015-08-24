#ifndef ITER_REVERSE_HPP_
#define ITER_REVERSE_HPP_

#include "internal/iterbase.hpp"

#include <utility>
#include <iterator>

namespace iter {
  namespace impl {
    template <typename Container>
    class Reverser;

    template <typename T, std::size_t N>
    class Reverser<T[N]>;
  }

  template <typename Container>
  impl::Reverser<Container> reversed(Container&&);

  template <typename T, std::size_t N>
  impl::Reverser<T[N]> reversed(T(&)[N]);
}

template <typename Container>
class iter::impl::Reverser {
 private:
  Container container;
  friend Reverser iter::reversed<Container>(Container&&);

  Reverser(Container&& in_container)
      : container(std::forward<Container>(in_container)) {}

 public:
  Reverser(Reverser&&) = default;
  class Iterator : public std::iterator<std::input_iterator_tag,
                       iterator_traits_deref<Container>> {
   private:
    reverse_iterator_type<Container> sub_iter;

   public:
    Iterator(reverse_iterator_type<Container>&& iter)
        : sub_iter{std::move(iter)} {}

    reverse_iterator_deref<Container> operator*() {
      return *this->sub_iter;
    }

    reverse_iterator_arrow<Container> operator->() {
      return apply_arrow(this->sub_iter);
    }

    Iterator& operator++() {
      ++this->sub_iter;
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    bool operator!=(const Iterator& other) const {
      return this->sub_iter != other.sub_iter;
    }

    bool operator==(const Iterator& other) const {
      return !(*this != other);
    }
  };

  Iterator begin() {
    return {this->container.rbegin()};
  }

  Iterator end() {
    return {this->container.rend()};
  }
};

template <typename Container>
iter::impl::Reverser<Container> iter::reversed(Container&& container) {
  return {std::forward<Container>(container)};
}

// specialization for statically allocated arrays

template <typename T, std::size_t N>
class iter::impl::Reverser<T[N]> {
 private:
  T* array;
  friend Reverser iter::reversed<T, N>(T(&)[N]);

  // Value constructor for use only in the reversed function
  Reverser(T* in_array) : array{in_array} {}

 public:
  Reverser(Reverser&&) = default;

  class Iterator : public std::iterator<std::input_iterator_tag, T> {
   private:
    T* sub_iter;

   public:
    Iterator(T* iter) : sub_iter{iter} {}

    T& operator*() {
      return *(this->sub_iter - 1);
    }

    T* operator->() {
      return (this->sub_iter - 1);
    }

    Iterator& operator++() {
      --this->sub_iter;
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    bool operator!=(const Iterator& other) const {
      return this->sub_iter != other.sub_iter;
    }

    bool operator==(const Iterator& other) const {
      return !(*this != other);
    }
  };

  Iterator begin() {
    return {this->array + N};
  }

  Iterator end() {
    return {this->array};
  }
};

template <typename T, std::size_t N>
iter::impl::Reverser<T[N]> iter::reversed(T(&array)[N]) {
  return {array};
}

#endif
