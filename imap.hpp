#ifndef IMAP__H__
#define IMAP__H__

#include "zip.hpp"

#include <utility>
#include <type_traits>

namespace iter {

    namespace detail {

    template <std::size_t Index, typename Functor, typename Tup>
    struct Expander {
        template <typename... Ts>
        static auto call(Functor&& f, Tup&& tup, Ts&&... args)
            -> decltype(Expander<Index-1, Functor, Tup>::call(
                    std::forward<Functor>(f),
                    std::forward<Tup>(tup),
                    std::get<Index-1>(tup),
                    std::forward<Ts>(args)...))
        {
            // recurse
            return Expander<Index-1, Functor, Tup>::call(
                    std::forward<Functor>(f),
                    std::forward<Tup>(tup),
                    std::get<Index-1>(tup), // pull out one element
                    std::forward<Ts>(args)...); // everything already expanded
        }
    };

    template <typename Functor, typename Tup>
    struct Expander<0, Functor, Tup> {
        template <typename... Ts>
        static auto call(Functor&& f, Tup&&, Ts&&... args)
            -> decltype(f(std::forward<Ts>(args)...))
        {
            static_assert(
                std::tuple_size<
                    typename std::remove_reference<Tup>::type>::value
                    == sizeof...(Ts),
                "tuple has not been fully expanded");
            return f(std::forward<Ts>(args)...); // the actual call
        }
    };

    template <typename Functor, typename Tup>
    auto call_with_tuple(Functor&& f, Tup&& tup)
        -> decltype(Expander<std::tuple_size<
                    typename std::remove_reference<Tup>::type>::value,
                    Functor, Tup>::call(
                        std::forward<Functor>(f),
                        std::forward<Tup>(tup)))
    {
        return Expander<std::tuple_size<
            typename std::remove_reference<Tup>::type>::value,
            Functor, Tup>::call(
                std::forward<Functor>(f),
                std::forward<Tup>(tup));
    }

    } // end detail

    //Forward declarations of IMap and imap
    template <typename MapFunc, typename... Containers>
    class IMap;

    template <typename MapFunc, typename... Containers>
    IMap<MapFunc, Containers...> imap(MapFunc, Containers&&...);

    template <typename MapFunc, typename... Containers>
    class IMap {
        // The imap function is the only thing allowed to create a IMap
        friend IMap imap<MapFunc, Containers...>(MapFunc, Containers&& ...);

        using ZippedIterType = iterator_type<Zipped<Containers...>>;
        private:
            MapFunc map_func;
            Zipped<Containers...> zipped;
            
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
                    mutable ZippedIterType zipiter;

                public:
                    Iterator(MapFunc map_func, ZippedIterType zipiter) :
                        map_func(map_func),
                        zipiter(zipiter)
                    { } 

                    auto operator*() -> 
                        decltype(detail::call_with_tuple(
                                    this->map_func, *(this->zipiter)))
                    {
                        return detail::call_with_tuple(
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

#endif //ifndef IMAP__H__
