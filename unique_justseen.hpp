#ifndef ITER_UNIQUE_JUSTSEEN_HPP
#define ITER_UNIQUE_JUSTSEEN_HPP

#include "groupby.hpp"
#include "imap.hpp"

#include <utility>
#include <iterator>
#include <initializer_list>

namespace iter {

  template <typename Container>
  auto unique_justseen(Container&& container) {
    // explicit return type in lambda so reference types are preserved
    return imap([](auto&& group) -> impl::iterator_deref<Container> {
      return *std::begin(group.second);
    }, groupby(std::forward<Container>(container)));
  }

  template <typename T>
  auto unique_justseen(std::initializer_list<T> il) {
    return imap(
        [](auto&& group) -> impl::iterator_deref<std::initializer_list<T>> {
          return *std::begin(group.second);
        },
        groupby(il));
  }
}

#endif
