#ifndef ITER_IMAP_H_
#define ITER_IMAP_H_

#include "zip.hpp"
#include "starmap.hpp"

#include <utility>

namespace iter {
  namespace impl {
    struct IMapFn : PipeableAndBindFirst<IMapFn> {
      template <typename MapFunc, typename... Containers>
      decltype(auto) operator()(
          MapFunc map_func, Containers&&... containers) const {
        return starmap(map_func, zip(std::forward<Containers>(containers)...));
      }
      using PipeableAndBindFirst<IMapFn>::operator();
    };
  }
  constexpr impl::IMapFn imap{};
}

#endif
