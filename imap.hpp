#ifndef ITER_IMAP_H_
#define ITER_IMAP_H_

#include "zip.hpp"

#include <utility>
#include <type_traits>
#include <tuple>

namespace iter {
  namespace detail {
    template <std::size_t Index, typename Functor, typename Tup>
    struct Expander {
      template <typename... Ts>
      static auto call(Functor&& f, Tup&& tup, Ts&&... args)
          -> decltype(Expander<Index - 1, Functor, Tup>::call(
              std::forward<Functor>(f), std::forward<Tup>(tup),
              std::forward<typename std::tuple_element<Index - 1,
                  typename std::remove_reference<Tup>::type>::type>(
                  std::get<Index - 1>(tup)),
              std::forward<Ts>(args)...)) {
        // recurse
        return Expander<Index - 1, Functor, Tup>::call(
            std::forward<Functor>(f), std::forward<Tup>(tup),
            // pull out one element
            std::forward<typename std::tuple_element<Index - 1,
                typename std::remove_reference<Tup>::type>::type>(
                std::get<Index - 1>(tup)),
            std::forward<Ts>(args)...);  // everything already expanded
      }
    };

    template <typename Functor, typename Tup>
    struct Expander<0, Functor, Tup> {
      template <typename... Ts>
      static auto call(Functor&& f, Tup&&, Ts&&... args)
          -> decltype(f(std::forward<Ts>(args)...)) {
        static_assert(
            std::tuple_size<typename std::remove_reference<Tup>::type>::value
                == sizeof...(Ts),
            "tuple has not been fully expanded");
        return f(std::forward<Ts>(args)...);  // the actual call
      }
    };

    template <typename Functor, typename Tup>
    auto call_with_tuple(Functor&& f, Tup&& tup) -> decltype(
        Expander<std::tuple_size<
                     typename std::remove_reference<Tup>::type>::value,
            Functor, Tup>::call(std::forward<Functor>(f),
            std::forward<Tup>(tup))) {
      return Expander<std::tuple_size<
                          typename std::remove_reference<Tup>::type>::value,
          Functor, Tup>::call(std::forward<Functor>(f), std::forward<Tup>(tup));
    }

  }  // end detail

  namespace impl {
    template <typename MapFunc, typename... Containers>
    class IMapper;
  }

  template <typename MapFunc, typename... Containers>
  impl::IMapper<MapFunc, Containers...> imap(MapFunc, Containers&&...);
}

template <typename MapFunc, typename... Containers>
class iter::impl::IMapper {
  // The imap function is the only thing allowed to create a IMapper
  friend IMapper iter::imap<MapFunc, Containers...>(MapFunc, Containers&&...);

  using ZippedType = decltype(zip(std::declval<Containers>()...));
  using ZippedIterType = iterator_type<ZippedType>;

 private:
  MapFunc map_func;
  ZippedType zipped;

  using IMapIterDeref =
      decltype(iter::detail::call_with_tuple(map_func, *std::begin(zipped)));

  IMapper(MapFunc in_map_func, Containers&&... in_containers)
      : map_func(in_map_func),
        zipped(zip(std::forward<Containers>(in_containers)...)) {}

 public:
  IMapper(IMapper&&) = default;

  class Iterator : public std::iterator<std::input_iterator_tag,
                       typename std::remove_reference<IMapIterDeref>::type> {
   private:
    MapFunc* map_func;
    ZippedIterType zipiter;

   public:
    Iterator(MapFunc& in_map_func, ZippedIterType&& in_zipiter)
        : map_func(&in_map_func), zipiter(std::move(in_zipiter)) {}

    IMapIterDeref operator*() {
      return iter::detail::call_with_tuple(*this->map_func, *(this->zipiter));
    }

    ArrowProxy<IMapIterDeref> operator->() {
      return {**this};
    }

    Iterator& operator++() {
      ++this->zipiter;
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    bool operator!=(const Iterator& other) const {
      return this->zipiter != other.zipiter;
    }

    bool operator==(const Iterator& other) const {
      return !(*this != other);
    }
  };

  Iterator begin() {
    return {this->map_func, this->zipped.begin()};
  }

  Iterator end() {
    return {this->map_func, this->zipped.end()};
  }
};

template <typename MapFunc, typename... Containers>
iter::impl::IMapper<MapFunc, Containers...> iter::imap(
    MapFunc map_func, Containers&&... containers) {
  return {map_func, std::forward<Containers>(containers)...};
}

#endif
