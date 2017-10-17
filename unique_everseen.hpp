#ifndef ITER_UNIQUE_EVERSEEN_HPP_
#define ITER_UNIQUE_EVERSEEN_HPP_

#include "filter.hpp"
#include "internal/iterbase.hpp"

#include <functional>
#include <iterator>
#include <type_traits>
#include <unordered_set>
#include <utility>

namespace iter {
  namespace impl {
    struct UniqueEverseenFn : Pipeable<UniqueEverseenFn> {
      template <typename Container>
      auto operator()(Container&& container) const {
        using elem_type = impl::iterator_deref<Container>;
        auto func = [elem_seen = std::unordered_set<std::decay_t<elem_type>>()](
            const std::remove_reference_t<elem_type>& e) mutable {
          return elem_seen.insert(e).second;
        };
        return filter(func, std::forward<Container>(container));
      }
    };
  }

  constexpr impl::UniqueEverseenFn unique_everseen{};
}

#endif
