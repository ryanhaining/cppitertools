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

  template <std::size_t I, typename TupleTypeT>
  using OptType = boost::optional<iterator_deref<
      std::tuple_element_t<I, std::remove_reference_t<TupleTypeT>>>>;

  template <std::size_t I, typename TupleTypeT>
  using ConstOptType = boost::optional<const_iterator_type_deref<
      std::tuple_element_t<I, std::remove_reference_t<TupleTypeT>>>>;

  template <typename TupleTypeT,
      template <std::size_t, typename> class OptTempl>
  using ZipIterDeref = std::tuple<OptTempl<Is, TupleTypeT>...>;

  ZippedLongest(TupleType&& containers) : containers_(std::move(containers)) {}

 public:
  ZippedLongest(ZippedLongest&&) = default;
  template <typename TupleTypeT, template <typename> class IterTuple,
      template <std::size_t, typename> class OptTempl>
  class Iterator {
   private:
    template <typename, template <typename> class,
        template <std::size_t, typename> class>
    friend class Iterator;
    IterTuple<TupleTypeT> iters_;
    IterTuple<TupleTypeT> ends_;

   public:
    using iterator_category = std::input_iterator_tag;
    using value_type = ZipIterDeref<TupleTypeT, OptTempl>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    Iterator(IterTuple<TupleTypeT>&& iters, IterTuple<TupleTypeT>&& ends)
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

    template <typename T, template <typename> class TT,
        template <std::size_t, typename> class TU>
    bool operator!=(const Iterator<T, TT, TU>& other) const {
      return (... || (std::get<Is>(iters_) != std::get<Is>(other.iters_)));
    }

    template <typename T, template <typename> class TT,
        template <std::size_t, typename> class TU>
    bool operator==(const Iterator<T, TT, TU>& other) const {
      return !(*this != other);
    }

    ZipIterDeref<TupleTypeT, OptTempl> operator*() {
      return {((std::get<Is>(iters_) != std::get<Is>(ends_))
                   ? OptTempl<Is, TupleTypeT>{*std::get<Is>(iters_)}
                   : OptTempl<Is, TupleTypeT>{})...};
    }

    auto operator-> () -> ArrowProxy<decltype(**this)> {
      return {**this};
    }
  };

  Iterator<TupleType, iterator_tuple_type, OptType> begin() {
    return {{get_begin(std::get<Is>(containers_))...},
        {get_end(std::get<Is>(containers_))...}};
  }

  Iterator<TupleType, iterator_tuple_type, OptType> end() {
    return {{get_end(std::get<Is>(containers_))...},
        {get_end(std::get<Is>(containers_))...}};
  }

  Iterator<AsConst<TupleType>, const_iterator_tuple_type, ConstOptType> begin()
      const {
    return {{get_begin(std::as_const(std::get<Is>(containers_)))...},
        {get_end(std::as_const(std::get<Is>(containers_)))...}};
  }

  Iterator<AsConst<TupleType>, const_iterator_tuple_type, ConstOptType> end()
      const {
    return {{get_end(std::as_const(std::get<Is>(containers_)))...},
        {get_end(std::as_const(std::get<Is>(containers_)))...}};
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
