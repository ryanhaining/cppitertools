#ifndef ITER_REVERSE_HPP_
#define ITER_REVERSE_HPP_

#include "internal/iterbase.hpp"

#include <utility>
#include <iterator>

namespace iter {
  namespace impl {
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

  using reverse_iterator_type =
      decltype(std::rbegin(std::declval<Container&>()));

  using reverse_iterator_deref =
      decltype(*std::declval<reverse_iterator_type&>());

  using reverse_iterator_traits_deref =
      std::remove_reference_t<reverse_iterator_deref>;

  using reverse_iterator_arrow = detail::arrow<reverse_iterator_type>;

 public:
  Reverser(Reverser&&) = default;
  class Iterator : public std::iterator<std::input_iterator_tag,
                       reverse_iterator_traits_deref> {
   private:
    reverse_iterator_type sub_iter;

   public:
    Iterator(reverse_iterator_type&& iter) : sub_iter{std::move(iter)} {}

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
