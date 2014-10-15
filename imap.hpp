#ifndef ITER_IMAP_H_
#define ITER_IMAP_H_

#include "zip.hpp"

#include <utility>

namespace iter {

    //Forward declarations of IMap and imap
    template <typename MapFunc, typename... Containers>
    class IMap;

    template <typename MapFunc, typename... Containers>
    IMap<MapFunc, Containers...> imap(MapFunc, Containers&&...);

    template <typename MapFunc, typename... Containers>
    class IMap {
        // The imap function is the only thing allowed to create a IMap
        friend IMap imap<MapFunc, Containers...>(MapFunc, Containers&& ...);

        using ZippedType = decltype(zip(std::declval<Containers>()...));
        using ZippedIterType = iterator_type<ZippedType>;
        private:
            MapFunc map_func;
            ZippedType zipped;
            
            // Value constructor for use only in the imap function
            IMap(MapFunc map_func, Containers&& ... containers) :
                map_func(map_func),
                zipped(zip(std::forward<Containers>(containers)...))
            { }
            IMap() = delete;
            IMap& operator=(const IMap&) = delete;

        public:
            IMap(const IMap&) = default;
            IMap(IMap&&) = default;

            class Iterator {
                private:
                    MapFunc map_func;
                    ZippedIterType zipiter;

                public:
                    Iterator(MapFunc map_func, ZippedIterType zipiter) :
                        map_func(map_func),
                        zipiter(zipiter)
                    { } 

                    auto operator*() {
                        return call_with_tuple(
                                this->map_func, *(this->zipiter));
                    }

                    Iterator& operator++() { 
                        ++this->zipiter;
                        return *this;
                    }

                    bool operator!=(const Iterator& other) const {
                        return this->zipiter != other.zipiter;
                    }
            };

            Iterator begin() {
                return {this->map_func, this->zipped.begin()};
            }

            Iterator end() {
                return {this->map_func, this->zipped.end()};
            }

    };

    // Helper function to instantiate a IMap
    template <typename MapFunc, typename... Containers>
    IMap<MapFunc, Containers...> imap(
            MapFunc map_func, Containers&& ... containers) {
        return {map_func, std::forward<Containers>(containers)...};
    }

}

#endif // #ifndef ITER_IMAP_H_
