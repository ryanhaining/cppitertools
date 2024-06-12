#ifndef ITER_IMAP_H_
#define ITER_IMAP_H_

#include <utility>

#include "starmap.hpp"
#include "zip.hpp"

namespace iter {
  namespace impl {
    struct IMapFn : PipeableAndBindFirst<IMapFn> {
      template <typename MapFunc, typename... Containers>
      auto operator()(MapFunc map_func, Containers&&... containers) const
          // explicitly specifying type here to allow more expressions that only
          // care about the type, and don't need a valid implementation.
          // See #66
          -> StarMapper<MapFunc,
              decltype(zip(std::forward<Containers>(containers)...))> {
        return starmap(map_func, zip(std::forward<Containers>(containers)...));
      }
      using PipeableAndBindFirst<IMapFn>::operator();
    };
  }
  constexpr impl::IMapFn imap{};
}

#endif
