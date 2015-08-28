#ifndef ITER_UNIQUE_EVERSEEN_HPP_
#define ITER_UNIQUE_EVERSEEN_HPP_

#include "internal/iterbase.hpp"
#include "filter.hpp"

#include <type_traits>
#include <functional>
#include <utility>
#include <unordered_set>
#include <iterator>

namespace iter {
  template <typename Container>
  auto unique_everseen(Container&& container) {
    using elem_type = impl::iterator_deref<Container>;
    auto func = [elem_seen = std::unordered_set<std::decay_t<elem_type>>()](
        const elem_type& e) mutable {
      return elem_seen.insert(e).second;
    };
    return filter(func, std::forward<Container>(container));
  }

  template <typename T>
  auto unique_everseen(std::initializer_list<T> il) {
    auto func = [elem_seen = std::unordered_set<T>()](const T& e) mutable {
      return elem_seen.insert(e).second;
    };
    return filter(func, il);
  }
}

#endif
