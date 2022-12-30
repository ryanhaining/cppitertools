#ifndef ITER_UNIQUE_JUSTSEEN_HPP
#define ITER_UNIQUE_JUSTSEEN_HPP

#include "groupby.hpp"
#include "imap.hpp"

#include <iterator>
#include <utility>

namespace iter {
  namespace impl {
    struct UniqueJustseenFn : Pipeable<UniqueJustseenFn> {
     public:
      template <typename Container, typename KeyFunc>
      auto operator()(Container&& container, KeyFunc key_fn) const {
        // decltype(auto) return type in lambda so reference types are preserved
        return imap(
            [](auto&& group) -> decltype(auto) {
              return *get_begin(group.second);
            },
            groupby(std::forward<Container>(container), key_fn));
      }

      template <typename Container>
      auto operator()(Container&& container) const {
        return (*this)(std::forward<Container>(container), Identity{});
      }
    };
  }
  constexpr impl::UniqueJustseenFn unique_justseen{};
}

#endif
