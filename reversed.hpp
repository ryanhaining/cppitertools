#ifndef ITER_REVERSE_HPP_
#define ITER_REVERSE_HPP_

#include "internal/iterator_wrapper.hpp"
#include "internal/iterbase.hpp"

#include <iterator>
#include <utility>

namespace iter {
  namespace impl {
    template <typename Container>
    using reverse_iterator_type =
        decltype(std::rbegin(std::declval<Container&>()));
    template <typename Container>
    using reverse_iterator_end_type =
        decltype(std::rend(std::declval<Container&>()));

    // If rbegin and rend return the same type, type will be
    // reverse_iterator_type<Container>
    // If rbegin and rend return different types, type will be
    // IteratorWrapperImpl
    template <typename Container, bool same_types>
    struct ReverseIteratorWrapperImplType;

    template <typename Container>
    struct ReverseIteratorWrapperImplType<Container, true>
        : type_is<reverse_iterator_type<Container>> {};

    template <typename Container>
    struct ReverseIteratorWrapperImplType<Container, false>
        : type_is<IteratorWrapperImpl<reverse_iterator_type<Container>,
              reverse_iterator_end_type<Container>>> {};

    template <typename Container>
    using ReverseIteratorWrapper =
        typename ReverseIteratorWrapperImplType<Container,
            std::is_same_v<impl::reverse_iterator_type<Container>,
                                                    impl::
                                                        reverse_iterator_end_type<Container>>>::
            type;

    template <typename Container>
    class Reverser;

    using ReversedFn = IterToolFn<Reverser>;
  }
  constexpr impl::ReversedFn reversed{};
}

template <typename Container>
class iter::impl::Reverser {
 private:
  Container container_;
  friend ReversedFn;

  Reverser(Container&& container)
      : container_(std::forward<Container>(container)) {}

  template <typename T>
  using reverse_iterator_deref =
      decltype(*std::declval<reverse_iterator_type<T>&>());

  template <typename T>
  using reverse_iterator_traits_deref =
      std::remove_reference_t<reverse_iterator_deref<T>>;

  template <typename T>
  using reverse_iterator_arrow = detail::arrow<reverse_iterator_type<T>>;

 public:
  Reverser(Reverser&&) = default;
  template <typename ContainerT>
  class Iterator {
   private:
    template <typename>
    friend class Iterator;
    ReverseIteratorWrapper<ContainerT> sub_iter_;

   public:
    using iterator_category = std::input_iterator_tag;
    using value_type = reverse_iterator_traits_deref<ContainerT>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    Iterator(ReverseIteratorWrapper<ContainerT>&& sub_iter)
        : sub_iter_{std::move(sub_iter)} {}

    reverse_iterator_deref<ContainerT> operator*() {
      return *sub_iter_;
    }

    reverse_iterator_arrow<ContainerT> operator->() {
      return apply_arrow(sub_iter_);
    }

    Iterator& operator++() {
      ++sub_iter_;
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    template <typename T>
    bool operator!=(const Iterator<T>& other) const {
      return sub_iter_ != other.sub_iter_;
    }

    template <typename T>
    bool operator==(const Iterator<T>& other) const {
      return !(*this != other);
    }
  };

  Iterator<Container> begin() {
    return {std::rbegin(container_)};
  }

  Iterator<Container> end() {
    return {std::rend(container_)};
  }

  Iterator<AsConst<Container>> begin() const {
    return {std::rbegin(std::as_const(container_))};
  }

  Iterator<AsConst<Container>> end() const {
    return {std::rend(std::as_const(container_))};
  }
};

#endif
