#ifndef ITER_ZIP_HPP_
#define ITER_ZIP_HPP_

#include "internal/iter_tuples.hpp"
#include "internal/iterbase.hpp"

#include <algorithm>
#include <iterator>
#include <tuple>
#include <utility>

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
  TupleType containers_;
  friend Zipped iter::impl::zip_impl<TupleType, Is...>(
      TupleType&&, std::index_sequence<Is...>);

  template <typename TupleTypeT>
  using ZipIterDeref = iterator_deref_tuple<TupleTypeT>;

  Zipped(TupleType&& containers) : containers_(std::move(containers)) {}

 public:
  Zipped(Zipped&&) = default;
  template <typename TupleTypeT>
  class Iterator : public std::iterator<std::input_iterator_tag, ZipIterDeref<TupleTypeT>> {
   private:
    template <typename> friend class Iterator;
    iterator_tuple_type<TupleTypeT> iters_;

   public:
    Iterator(iterator_tuple_type<TupleTypeT>&& iters)
        : iters_(std::move(iters)) {}

    Iterator& operator++() {
      absorb(++std::get<Is>(iters_)...);
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    template <typename T>
    bool operator!=(const Iterator<T>& other) const {
      if (sizeof...(Is) == 0) return false;

      bool results[] = {
          true, (std::get<Is>(iters_) != std::get<Is>(other.iters_))...};
      return std::all_of(
          get_begin(results), get_end(results), [](bool b) { return b; });
    }

    template <typename T>
    bool operator==(const Iterator<T>& other) const {
      return !(*this != other);
    }

    ZipIterDeref<TupleTypeT> operator*() {
      return ZipIterDeref<TupleTypeT>{(*std::get<Is>(iters_))...};
    }

    auto operator-> () -> ArrowProxy<decltype(**this)> {
      return {**this};
    }
  };

  Iterator<TupleType> begin() {
    return {iterator_tuple_type<TupleType>{
        get_begin(std::get<Is>(containers_))...}};
  }

  Iterator<TupleType> end() {
    return {
        iterator_tuple_type<TupleType>{get_end(std::get<Is>(containers_))...}};
  }

  Iterator<AsConst<TupleType>> begin() const {
    return {iterator_tuple_type<AsConst<TupleType>>{
        get_begin(std::get<Is>(as_const(containers_)))...}};
  }

  Iterator<AsConst<TupleType>> end() const {
    return {
        iterator_tuple_type<AsConst<TupleType>>{get_end(std::get<Is>(as_const(containers_)))...}};
  }
};

template <typename TupleType, std::size_t... Is>
iter::impl::Zipped<TupleType, Is...> iter::impl::zip_impl(
    TupleType&& containers, std::index_sequence<Is...>) {
  return {std::move(containers)};
}

template <typename... Containers>
auto iter::zip(Containers&&... containers) {
  return impl::zip_impl(
      std::tuple<Containers...>{std::forward<Containers>(containers)...},
      std::index_sequence_for<Containers...>{});
}

#endif
