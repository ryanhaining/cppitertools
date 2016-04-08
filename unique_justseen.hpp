#ifndef ITER_UNIQUE_JUSTSEEN_HPP
#define ITER_UNIQUE_JUSTSEEN_HPP

#include "groupby.hpp"
#include "imap.hpp"

#include <utility>
#include <iterator>

namespace iter {

  template <typename Container>
  auto unique_justseen(Container&& container) {
    // explicit return type in lambda so reference types are preserved
    return imap([](auto&& group) -> impl::iterator_deref<Container> {
      return *std::begin(group.second);
    }, groupby(std::forward<Container>(container)));
  }
}

#endif
