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
     private:
      template <typename Container>
      using Key = std::decay_t<iterator_deref<Container>>;

     public:
      template <typename Container, typename Hash, typename KeyEqual>
      auto operator()(Container&& container, const Hash& hash,
          const KeyEqual& key_equal) const {
        // You can't pass a hash function or an equality function without
        // passing a bucket_count as well. We get the default bucket count here
        // the first time this function runs.
        static auto default_bucket_count =
            std::unordered_set<char>{}.bucket_count();
        using elem_type = iterator_deref<Container>;
        auto func =
            [elem_seen =
                    std::unordered_set<std::decay_t<elem_type>, Hash, KeyEqual>(
                        default_bucket_count, hash, key_equal)](
                const std::remove_reference_t<elem_type>& e) mutable {
              return elem_seen.insert(e).second;
            };
        return filter(func, std::forward<Container>(container));
      }

      template <typename Container>
      auto operator()(Container&& container) const {
        return (*this)(std::forward<Container>(container),
            std::hash<Key<Container>>{}, std::equal_to<Key<Container>>{});
      }
    };
  }

  constexpr impl::UniqueEverseenFn unique_everseen{};
}

#endif
