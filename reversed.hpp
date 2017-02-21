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
            std::is_same<impl::reverse_iterator_type<Container>,
                impl::reverse_iterator_end_type<Container>>{}>::type;

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

  using reverse_iterator_deref =
      decltype(*std::declval<reverse_iterator_type<Container>&>());

  using reverse_iterator_traits_deref =
      std::remove_reference_t<reverse_iterator_deref>;

  using reverse_iterator_arrow =
      detail::arrow<reverse_iterator_type<Container>>;

 public:
  Reverser(Reverser&&) = default;
  class Iterator : public std::iterator<std::input_iterator_tag,
                       reverse_iterator_traits_deref> {
   private:
    ReverseIteratorWrapper<Container> sub_iter_;

   public:
    Iterator(ReverseIteratorWrapper<Container>&& sub_iter)
        : sub_iter_{std::move(sub_iter)} {}

    reverse_iterator_deref operator*() {
      return *sub_iter_;
    }

    reverse_iterator_arrow operator->() {
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

    bool operator!=(const Iterator& other) const {
      return sub_iter_ != other.sub_iter_;
    }

    bool operator==(const Iterator& other) const {
      return !(*this != other);
    }
  };

  Iterator begin() {
    return {std::rbegin(container_)};
  }

  Iterator end() {
    return {std::rend(container_)};
  }
};

#endif
