#ifndef ITER_ZIP_HPP_
#define ITER_ZIP_HPP_

#include "internal/iter_tuples.hpp"
#include "internal/iterbase.hpp"

#include <iterator>
#include <tuple>
#include <utility>
#include <algorithm>

namespace iter {
  namespace impl {
    template <typename TupleType, std::size_t... Is>
    class Zipped;

    template <typename TupleType, std::size_t... Is>
    Zipped<TupleType, Is...> zip_impl(TupleType&&, std::index_sequence<Is...>);
  }

  template <typename... Containers>
  auto zip(Containers&&... containers);
}

template <typename TupleType, std::size_t... Is>
class iter::impl::Zipped {
 private:
  TupleType containers;
  friend Zipped iter::impl::zip_impl<TupleType, Is...>(
      TupleType&&, std::index_sequence<Is...>);

  using ZipIterDeref = iterator_deref_tuple<TupleType>;

  Zipped(TupleType&& in_containers) : containers(std::move(in_containers)) {}

 public:
  Zipped(Zipped&&) = default;
  class Iterator : public std::iterator<std::input_iterator_tag, ZipIterDeref> {
   private:
    iterator_tuple_type<TupleType> iters;

   public:
    Iterator(iterator_tuple_type<TupleType>&& its) : iters(std::move(its)) {}

    Iterator& operator++() {
      absorb(++std::get<Is>(this->iters)...);
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
          true, (std::get<Is>(this->iters) != std::get<Is>(other.iters))...};
      return std::all_of(
          std::begin(results), std::end(results), [](bool b) { return b; });
    }

    bool operator==(const Iterator& other) const {
      return !(*this != other);
    }

    ZipIterDeref operator*() {
      return ZipIterDeref{(*std::get<Is>(this->iters))...};
    }

    auto operator-> () -> ArrowProxy<decltype(**this)> {
      return {**this};
    }
  };

  Iterator begin() {
    return {iterator_tuple_type<TupleType>{
        std::begin(std::get<Is>(this->containers))...}};
  }

  Iterator end() {
    return {iterator_tuple_type<TupleType>{
        std::end(std::get<Is>(this->containers))...}};
  }
};

template <typename TupleType, std::size_t... Is>
iter::impl::Zipped<TupleType, Is...> iter::impl::zip_impl(
    TupleType&& in_containers, std::index_sequence<Is...>) {
  return {std::move(in_containers)};
}

template <typename... Containers>
auto iter::zip(Containers&&... containers) {
  return impl::zip_impl(
      std::tuple<Containers...>{std::forward<Containers>(containers)...},
      std::index_sequence_for<Containers...>{});
}

#endif
