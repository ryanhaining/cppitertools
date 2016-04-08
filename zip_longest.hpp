#ifndef ITER_ZIP_LONGEST_HPP_
#define ITER_ZIP_LONGEST_HPP_

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
  TupleType containers;
  friend ZippedLongest zip_longest_impl<TupleType, Is...>(
      TupleType&&, std::index_sequence<Is...>);

  template <std::size_t I>
  using OptType =
      boost::optional<iterator_deref<std::tuple_element_t<I, TupleType>>>;

  using ZipIterDeref = std::tuple<OptType<Is>...>;

  ZippedLongest(TupleType&& in_containers)
      : containers(std::move(in_containers)) {}

 public:
  ZippedLongest(ZippedLongest&&) = default;
  class Iterator : public std::iterator<std::input_iterator_tag, ZipIterDeref> {
   private:
    iterator_tuple_type<TupleType> iters;
    iterator_tuple_type<TupleType> ends;

   public:
    Iterator(iterator_tuple_type<TupleType>&& in_iters,
        iterator_tuple_type<TupleType>&& in_ends)
        : iters(std::move(in_iters)), ends(std::move(in_ends)) {}

    Iterator& operator++() {
      // increment every iterator that's not already at
      // the end
      absorb(((std::get<Is>(this->iters) != std::get<Is>(this->ends))
                  ? (++std::get<Is>(this->iters), 0)
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
          false, (std::get<Is>(this->iters) != std::get<Is>(other.iters))...};
      return std::any_of(
          std::begin(results), std::end(results), [](bool b) { return b; });
    }

    bool operator==(const Iterator& other) const {
      return !(*this != other);
    }

    ZipIterDeref operator*() {
      return ZipIterDeref{
          ((std::get<Is>(this->iters) != std::get<Is>(this->ends))
                  ? OptType<Is>{*std::get<Is>(this->iters)}
                  : OptType<Is>{})...};
    }

    auto operator-> () -> ArrowProxy<decltype(**this)> {
      return {**this};
    }
  };

  Iterator begin() {
    return {iterator_tuple_type<TupleType>{
                std::begin(std::get<Is>(this->containers))...},
        iterator_tuple_type<TupleType>{
            std::end(std::get<Is>(this->containers))...}};
  }

  Iterator end() {
    return {iterator_tuple_type<TupleType>{
                std::end(std::get<Is>(this->containers))...},
        iterator_tuple_type<TupleType>{
            std::end(std::get<Is>(this->containers))...}};
  }
};

template <typename TupleType, std::size_t... Is>
iter::impl::ZippedLongest<TupleType, Is...> iter::impl::zip_longest_impl(
    TupleType&& in_containers, std::index_sequence<Is...>) {
  return {std::move(in_containers)};
}

template <typename... Containers>
auto iter::zip_longest(Containers&&... containers) {
  return impl::zip_longest_impl(
      std::tuple<Containers...>{std::forward<Containers>(containers)...},
      std::index_sequence_for<Containers...>{});
}

#endif
