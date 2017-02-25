#ifndef ITER_ZIP_LONGEST_HPP_
#define ITER_ZIP_LONGEST_HPP_

#include "internal/iter_tuples.hpp"
#include "internal/iterbase.hpp"

#include <boost/optional.hpp>
#include <iterator>
#include <tuple>
#include <utility>

namespace iter {
  namespace impl {
    template <typename TupleType, std::size_t... Is>
    class ZippedLongest;

    template <typename TupleType, std::size_t... Is>
    ZippedLongest<TupleType, Is...> zip_longest_impl(
        TupleType&&, std::index_sequence<Is...>);
  }

  template <typename... Containers>
  auto zip_longest(Containers&&... containers);
}

template <typename TupleType, std::size_t... Is>
class iter::impl::ZippedLongest {
 private:
  TupleType containers_;
  friend ZippedLongest zip_longest_impl<TupleType, Is...>(
      TupleType&&, std::index_sequence<Is...>);

  template <std::size_t I>
  using OptType =
      boost::optional<iterator_deref<std::tuple_element_t<I, TupleType>>>;

  using ZipIterDeref = std::tuple<OptType<Is>...>;

  ZippedLongest(TupleType&& containers) : containers_(std::move(containers)) {}

 public:
  ZippedLongest(ZippedLongest&&) = default;
  class Iterator : public std::iterator<std::input_iterator_tag, ZipIterDeref> {
   private:
    iterator_tuple_type<TupleType> iters_;
    iterator_tuple_type<TupleType> ends_;

   public:
    Iterator(iterator_tuple_type<TupleType>&& iters,
        iterator_tuple_type<TupleType>&& ends)
        : iters_(std::move(iters)), ends_(std::move(ends)) {}

    Iterator& operator++() {
      // increment every iterator that's not already at
      // the end
      absorb(((std::get<Is>(iters_) != std::get<Is>(ends_))
                  ? (++std::get<Is>(iters_), 0)
                  : 0)...);
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    bool operator!=(const Iterator& other) const {
      if (sizeof...(Is) == 0) return false;

      bool results[] = {
          false, (std::get<Is>(iters_) != std::get<Is>(other.iters_))...};
      return std::any_of(
          get_begin(results), get_end(results), [](bool b) { return b; });
    }

    bool operator==(const Iterator& other) const {
      return !(*this != other);
    }

    ZipIterDeref operator*() {
      return ZipIterDeref{((std::get<Is>(iters_) != std::get<Is>(ends_))
                               ? OptType<Is>{*std::get<Is>(iters_)}
                               : OptType<Is>{})...};
    }

    auto operator-> () -> ArrowProxy<decltype(**this)> {
      return {**this};
    }
  };

  Iterator begin() {
    return {
        iterator_tuple_type<TupleType>{get_begin(std::get<Is>(containers_))...},
        iterator_tuple_type<TupleType>{get_end(std::get<Is>(containers_))...}};
  }

  Iterator end() {
    return {
        iterator_tuple_type<TupleType>{get_end(std::get<Is>(containers_))...},
        iterator_tuple_type<TupleType>{get_end(std::get<Is>(containers_))...}};
  }
};

template <typename TupleType, std::size_t... Is>
iter::impl::ZippedLongest<TupleType, Is...> iter::impl::zip_longest_impl(
    TupleType&& containers, std::index_sequence<Is...>) {
  return {std::move(containers)};
}

template <typename... Containers>
auto iter::zip_longest(Containers&&... containers) {
  return impl::zip_longest_impl(
      std::tuple<Containers...>{std::forward<Containers>(containers)...},
      std::index_sequence_for<Containers...>{});
}

#endif
