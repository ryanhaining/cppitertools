#ifndef ITER_UNIQUE_JUSTSEEN_HPP
#define ITER_UNIQUE_JUSTSEEN_HPP

#include "groupby.hpp"
#include "imap.hpp"

#include <iterator>
#include <utility>

namespace iter {
  namespace impl {
    struct UniqueJustseenFn
        : PipeableAndBindOptionalSecond<UniqueJustseenFn, Identity> {
     public:
      using PipeableAndBindOptionalSecond<UniqueJustseenFn, Identity>::
      operator();
      template <typename Container, typename KeyFunc>
      auto operator()(Container&& container, KeyFunc key_fn) const {
        // decltype(auto) return type in lambda so reference types are preserved
        return imap(
            [](auto&& group) -> decltype(auto) {
              return *get_begin(group.second);
            },
            groupby(std::forward<Container>(container), std::move(key_fn)));
      }
    };
  }
  inline constexpr impl::UniqueJustseenFn unique_justseen{};
}

#endif
