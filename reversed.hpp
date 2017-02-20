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
  Container container;
  friend ReversedFn;

  Reverser(Container&& in_container)
      : container(std::forward<Container>(in_container)) {}

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
    ReverseIteratorWrapper<Container> sub_iter;

   public:
    Iterator(ReverseIteratorWrapper<Container>&& iter)
        : sub_iter{std::move(iter)} {}

    reverse_iterator_deref operator*() {
      return *this->sub_iter;
    }

    reverse_iterator_arrow operator->() {
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
    return {std::rbegin(this->container)};
  }

  Iterator end() {
    return {std::rend(this->container)};
  }
};

#endif
