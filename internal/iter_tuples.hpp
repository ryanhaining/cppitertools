#ifndef ITERTOOLS_ITER_TUPLES_HPP_
#define ITERTOOLS_ITER_TUPLES_HPP_

#include "iterator_wrapper.hpp"
#include "iterbase.hpp"

namespace iter {
  namespace impl {
    namespace detail {
      template <typename... Ts>
      std::tuple<iterator_deref<Ts>...> iterator_tuple_deref_helper(
          const std::tuple<Ts...>&);
    }

    namespace detail {
      template <typename... Ts>
      std::tuple<IteratorWrapper<Ts>...> iterator_tuple_type_helper(
          const std::tuple<Ts...>&);
    }
    // Given a tuple template argument, evaluates to a tuple of iterators
    // for the template argument's contained types.
    template <typename TupleType>
    using iterator_tuple_type =
        decltype(detail::iterator_tuple_type_helper(std::declval<TupleType>()));

    // Given a tuple template argument, evaluates to a tuple of
    // what the iterators for the template argument's contained types
    // dereference to
    template <typename TupleType>
    using iterator_deref_tuple = decltype(
        detail::iterator_tuple_deref_helper(std::declval<TupleType>()));

    // function absorbing all arguments passed to it. used when
    // applying a function to a parameter pack but not passing the evaluated
    // results anywhere
    template <typename... Ts>
    void absorb(Ts&&...) {}

    namespace detail {
      template <typename Func, typename TupleType, std::size_t... Is>
      decltype(auto) call_with_tuple_impl(
          Func&& mf, TupleType&& tup, std::index_sequence<Is...>) {
        return mf(std::forward<std::tuple_element_t<Is,
                std::remove_reference_t<TupleType>>>(std::get<Is>(tup))...);
      }
    }

    // expand a TupleType into individual arguments when calling a Func
    template <typename Func, typename TupleType>
    decltype(auto) call_with_tuple(Func&& mf, TupleType&& tup) {
      constexpr auto TUP_SIZE = std::tuple_size<std::decay_t<TupleType>>::value;
      return detail::call_with_tuple_impl(std::forward<Func>(mf),
          std::forward<TupleType>(tup), std::make_index_sequence<TUP_SIZE>{});
    }
  }
}

#endif
