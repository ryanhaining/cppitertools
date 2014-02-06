#ifndef IMAP__H__
#define IMAP__H__

#include "zip.hpp"

#include <utility>
#include <type_traits>

namespace iter {

    // Everything in detail namespace 
    // modified from http://stackoverflow.com/questions/10766112/
    // Question by Thomas http://stackoverflow.com/users/115355/thomas
    // Answer by Kerrek SB http://stackoverflow.com/users/596781/kerrek-sb
    namespace detail
    {
        // implementation details, users never invoke these directly
        template <typename F, typename Tuple, bool Done, int Total, int... N>
        struct call_impl
        {
            static auto call(F f, Tuple && t) -> 
                decltype(call_impl<F,
                    Tuple,
                    Total == 1 + sizeof...(N),
                    Total,
                    N...,
                    sizeof...(N)>::call(f, std::forward<Tuple>(t)))
            {
                return call_impl<F,
                    Tuple,
                    Total == 1 + sizeof...(N),
                    Total,
                    N...,
                    sizeof...(N)>::call(f, std::forward<Tuple>(t));
            }
        };

        template <typename F, typename Tuple, int Total, int... N>
        struct call_impl<F, Tuple, true, Total, N...>
        {
            static auto call(F f, Tuple && t) -> 
                decltype(f(std::get<N>(std::forward<Tuple>(t))...))
            {
                return f(std::get<N>(std::forward<Tuple>(t))...);
            }
        };

        // user invokes this
        template <typename F, typename Tuple>
        auto call(F f, Tuple && t) -> 
            decltype(call_impl<F,
                Tuple,
                0 == std::tuple_size<typename std::decay<Tuple>::type>::value,
                std::tuple_size<typename std::decay<Tuple>::type>::value>
                ::call(f,std::forward<Tuple>(t)))
        {
            typedef typename std::decay<Tuple>::type ttype;
            return call_impl<F,
                Tuple,
                0 == std::tuple_size<ttype>::value,
                std::tuple_size<ttype>::value>::call(f,std::forward<Tuple>(t));
        }
    }

    //Forward declarations of IMap and imap
    template <typename MapFunc, typename... Containers>
    class IMap;

    template <typename MapFunc, typename... Containers>
    IMap<MapFunc, Containers...> imap(MapFunc, Containers &&...);

    template <typename MapFunc, typename... Containers>
    class IMap {
        // The imap function is the only thing allowed to create a IMap
        friend IMap imap<MapFunc, Containers...>(MapFunc, Containers && ...);

        // The type returned when dereferencing the Containers...::Iterator
        // XXX depends on zip using iterator_range.  would be nice if it didn't
        using Zipped = 
		  iterator_range<
		  zip_iter<typename
				   iter::weakest_iterator<decltype(std::declval<Containers>().begin())...>::value,
				   decltype(std::declval<Containers>().begin())...>>;

        using ZippedIterType = decltype(std::declval<Zipped>().begin());
            //typename std::remove_const<decltype(std::declval<Zipped>().begin())>::type;

        private:
            MapFunc map_func;
            Zipped zipped;
            
            // Value constructor for use only in the imap function
            IMap(MapFunc map_func, Containers && ... containers) :
                map_func(map_func),
                zipped(zip(std::forward<Containers>(containers)...))
            { }
            IMap () = delete;
            IMap & operator=(const IMap &) = delete;

        public:
            IMap (const IMap &) = default;

            class Iterator {
                private:
                    MapFunc map_func;
                    mutable ZippedIterType zipiter;

                public:
                    Iterator (MapFunc map_func, ZippedIterType zipiter) :
                        map_func(map_func),
                        zipiter(zipiter)
                    { } 

			  //template<typename U = zipiter::value_type> bool f(Type* x);
			  //template<typename... List> bool f(std::tuple<List...>* x);
                    auto operator*() const -> 
					  decltype(detail::call(this->map_func, (*(this->zipiter)).asTuple()))
                    {
					  return detail::call(this->map_func, (*(this->zipiter)).asTuple());
                    }

                    Iterator & operator++() { 
                        ++this->zipiter;
                        return *this;
                    }

                    bool operator!=(const Iterator & other) const {
                        return this->zipiter != other.zipiter;
                    }
            };

            Iterator begin() const {
                return {this->map_func, this->zipped.begin()};
            }

            Iterator end() const {
                return {this->map_func, this->zipped.end()};
            }

    };

    // Helper function to instantiate a IMap
    template <typename MapFunc, typename... Containers>
    IMap<MapFunc, Containers...> imap(
            MapFunc map_func, Containers && ... containers) {
        return {map_func, std::forward<Containers>(containers)...};
    }

}

#endif //ifndef IMAP__H__
