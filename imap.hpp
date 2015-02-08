#ifndef ITER_IMAP_H_
#define ITER_IMAP_H_

#include "zip.hpp"
#include "starmap.hpp"

#include <utility>

namespace iter {
    template <typename MapFunc, typename... Containers>
    decltype(auto) imap(MapFunc map_func, Containers&& ... containers) {
        return starmap(map_func,
                zip(std::forward<Containers>(containers)...));
    }
}

#endif
