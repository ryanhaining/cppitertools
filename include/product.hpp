#ifndef ITER_PRODUCT_HPP_
#define ITER_PRODUCT_HPP_

#include "internal/iter_tuples.hpp"
#include "internal/iterator_wrapper.hpp"
#include "internal/iterbase.hpp"

#include <array>
#include <iterator>
#include <tuple>
#include <utility>

namespace iter {
  namespace impl {
    template <typename TupleType, std::size_t... Is>
    class Productor;

    template <typename TupleType, std::size_t... Is>
    Productor<TupleType, Is...> product_impl(
        TupleType&& containers, std::index_sequence<Is...>);
  }
}

template <typename TupleType, std::size_t... Is>
class iter::impl::Productor {
  friend Productor iter::impl::product_impl<TupleType, Is...>(
      TupleType&&, std::index_sequence<Is...>);

 private:
  TupleType containers_;

  Productor(TupleType&& containers) : containers_(std::move(containers)) {}

 public:
  Productor(Productor&&) = default;

 private:
  template <typename IterTupType>
  class IteratorData {
    IteratorData() = delete;
    static_assert(
        std::tuple_size<std::decay_t<IterTupType>>::value == sizeof...(Is),
        "tuple size != sizeof Is");

   public:
    template <std::size_t Idx>
    static bool equal(const IterTupType& lhs, const IterTupType& rhs) {
      return !(std::get<Idx>(lhs) != std::get<Idx>(rhs));
    }

    // returns true if incremented, false if wrapped around
    template <std::size_t Idx>
    static bool get_and_increment_with_wraparound(IterTupType& iters,
        const IterTupType& begin_iters, const IterTupType& end_iters) {
      // if already at the end, we're looking at an empty container
      if (equal<Idx>(iters, end_iters)) {
        return false;
      }

      ++std::get<Idx>(iters);

      if (equal<Idx>(iters, end_iters)) {
        std::get<Idx>(iters) = std::get<Idx>(begin_iters);
        return false;
      }

      return true;
    }
    using IncFunc = bool (*)(
        IterTupType&, const IterTupType&, const IterTupType&);

    constexpr static std::array<IncFunc, sizeof...(Is)> incrementers{
        {get_and_increment_with_wraparound<Is>...}};
  };

  // template templates here because I need to defer evaluation in the const
  // iteration case for types that don't have non-const begin() and end(). If I
  // passed in the actual types of the tuples of iterators and the type for
  // deref they'd need to be known in the function declarations below.
  template <typename TupleTypeT, template <typename> class IteratorTuple,
      template <typename> class TupleDeref>
  class IteratorTempl {
#if NO_GCC_FRIEND_ERROR
   private:
    template <typename, template <typename> class, template <typename> class>
    friend class IteratorTempl;
#else
   public:
#endif

    using IterTupType = IteratorTuple<TupleTypeT>;
    IterTupType iters_;
    IterTupType begin_iters_;
    IterTupType end_iters_;

   public:
    using iterator_category = std::input_iterator_tag;
    using value_type = TupleDeref<TupleTypeT>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    IteratorTempl(IteratorTuple<TupleTypeT>&& iters,
        IteratorTuple<TupleTypeT>&& end_iters)
        : iters_(std::move(iters)),
          begin_iters_(iters_),
          end_iters_(std::move(end_iters)) {}

    IteratorTempl& operator++() {
      static constexpr int NUM_ELEMENTS = sizeof...(Is);
      bool performed_increment = false;
      for (int i = NUM_ELEMENTS - 1; i >= 0; --i) {
        if (IteratorData<IterTupType>::incrementers[i](
                iters_, begin_iters_, end_iters_)) {
          performed_increment = true;
          break;
        }
      }
      if (!performed_increment) {
        iters_ = end_iters_;
      }
      return *this;
    }

    IteratorTempl operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    template <typename T, template <typename> class IT,
        template <typename> class TD>
    bool operator!=(const IteratorTempl<T, IT, TD>& other) const {
      if constexpr (sizeof...(Is) == 0) {
        return false;
      } else {
        return (... && (std::get<Is>(iters_) != std::get<Is>(other.iters_)));
      }
    }

    template <typename T, template <typename> class IT,
        template <typename> class TD>
    bool operator==(const IteratorTempl<T, IT, TD>& other) const {
      return !(*this != other);
    }

    TupleDeref<TupleTypeT> operator*() {
      return {(*std::get<Is>(iters_))...};
    }

    auto operator-> () -> ArrowProxy<decltype(**this)> {
      return {**this};
    }
  };

  using Iterator =
      IteratorTempl<TupleType, iterator_tuple_type, iterator_deref_tuple>;
  using ConstIterator = IteratorTempl<AsConst<TupleType>,
      const_iterator_tuple_type, const_iterator_deref_tuple>;

 public:
  Iterator begin() {
    return {{get_begin(std::get<Is>(containers_))...},
        {get_end(std::get<Is>(containers_))...}};
  }

  Iterator end() {
    return {{get_end(std::get<Is>(containers_))...},
        {get_end(std::get<Is>(containers_))...}};
  }

  ConstIterator begin() const {
    return {{get_begin(std::as_const(std::get<Is>(containers_)))...},
        {get_end(std::as_const(std::get<Is>(containers_)))...}};
  }

  ConstIterator end() const {
    return {{get_end(std::as_const(std::get<Is>(containers_)))...},
        {get_end(std::as_const(std::get<Is>(containers_)))...}};
  }
};

namespace iter::impl {
  template <typename TupleType, std::size_t... Is>
  Productor<TupleType, Is...> product_impl(
      TupleType&& containers, std::index_sequence<Is...>) {
    return {std::move(containers)};
  }
}

namespace iter {
  template <typename... Containers>
  decltype(auto) product(Containers&&... containers) {
    return impl::product_impl(
        std::tuple<Containers...>(std::forward<Containers>(containers)...),
        std::index_sequence_for<Containers...>{});
  }

  constexpr std::array<std::tuple<>, 1> product() {
    return {{}};
  }
}

namespace iter::impl {
  // rvalue must be copied, lvalue and const lvalue references can be bound
  template <std::size_t... Is, typename Container>
  decltype(auto) product_repeat(
      std::index_sequence<Is...>, Container&& container) {
    return product(((void)Is, Container(container))...);
  }

  template <std::size_t... Is, typename Container>
  decltype(auto) product_repeat(
      std::index_sequence<Is...>, Container& container) {
    return product(((void)Is, container)...);
  }

  template <std::size_t... Is, typename Container>
  decltype(auto) product_repeat(
      std::index_sequence<Is...>, const Container& container) {
    return product(((void)Is, container)...);
  }
}

namespace iter {
  template <std::size_t N, typename Container>
  decltype(auto) product(Container&& container) {
    return impl::product_repeat(
        std::make_index_sequence<N>{}, std::forward<Container>(container));
  }
}

#endif
