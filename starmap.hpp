#ifndef ITER_STARMAP_H_
#define ITER_STARMAP_H_

#include "internal/iterbase.hpp"

#include <utility>
#include <iterator>
#include <type_traits>
#include <array>
#include <cassert>
#include <memory>

namespace iter {
  namespace impl {
    template <typename Func, typename Container>
    class StarMapper;

    template <typename Func, typename Container>
    StarMapper<Func, Container> starmap_helper(
        Func, Container&&, std::false_type);

    template <typename Func, typename Container>
    auto starmap_helper(Func, Container&&, std::true_type);

    template <typename Func, typename TupType, std::size_t... Is>
    class TupleStarMapper;

    template <typename Func, typename TupType, std::size_t... Is>
    TupleStarMapper<Func, TupType, Is...> starmap_with_tuples(
        Func, TupType&&, std::index_sequence<Is...>);
  }

  template <typename Func, typename Seq>
  auto starmap(Func func, Seq&& sequence);
}

// starmap with a container<T> where T is one of tuple, pair, array
template <typename Func, typename Container>
class iter::impl::StarMapper {
 private:
  Func func;
  Container container;

  using StarIterDeref = std::remove_reference_t<decltype(
      call_with_tuple(func, std::declval<iterator_deref<Container>>()))>;

  StarMapper(Func f, Container&& c)
      : func(std::move(f)), container(std::forward<Container>(c)) {}
  friend StarMapper starmap_helper<Func, Container>(
      Func, Container&&, std::false_type);

 public:
  class Iterator
      : public std::iterator<std::input_iterator_tag, StarIterDeref> {
   private:
    Func* func;
    iterator_type<Container> sub_iter;

   public:
    Iterator(Func& f, iterator_type<Container>&& iter)
        : func(&f), sub_iter(std::move(iter)) {}

    bool operator!=(const Iterator& other) const {
      return this->sub_iter != other.sub_iter;
    }

    bool operator==(const Iterator& other) const {
      return !(*this != other);
    }

    Iterator& operator++() {
      ++this->sub_iter;
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    decltype(auto) operator*() {
      return call_with_tuple(*this->func, *this->sub_iter);
    }

    auto operator-> () -> ArrowProxy<decltype(**this)> {
      return {**this};
    }
  };

  Iterator begin() {
    return {this->func, std::begin(this->container)};
  }

  Iterator end() {
    return {this->func, std::end(this->container)};
  }
};

// starmap for a tuple or pair of tuples or pairs
template <typename Func, typename TupType, std::size_t... Is>
class iter::impl::TupleStarMapper {
 private:
  Func func;
  TupType tup;

 private:
  static_assert(sizeof...(Is) == std::tuple_size<std::decay_t<TupType>>::value,
      "tuple size doesn't match size of Is");

  friend TupleStarMapper starmap_with_tuples<Func, TupType, Is...>(
      Func, TupType&&, std::index_sequence<Is...>);

  template <std::size_t Idx>
  static decltype(auto) get_and_call_with_tuple(Func& f, TupType& t) {
    return call_with_tuple(f, std::get<Idx>(t));
  }

  using ResultType = decltype(get_and_call_with_tuple<0>(func, tup));
  using CallerFunc = ResultType (*)(Func&, TupType&);

  constexpr static std::array<CallerFunc, sizeof...(Is)> callers{
      {get_and_call_with_tuple<Is>...}};

  using TraitsValue = std::remove_reference_t<ResultType>;

  TupleStarMapper(Func f, TupType t)
      : func(std::move(f)), tup(std::forward<TupType>(t)) {}

 public:
  class Iterator : public std::iterator<std::input_iterator_tag, TraitsValue> {
   private:
    Func* func;
    std::remove_reference_t<TupType>* tup;
    std::size_t index;

   public:
    Iterator(Func& f, TupType& t, std::size_t i)
        : func{&f}, tup{&t}, index{i} {}

    decltype(auto) operator*() {
      return callers[this->index](*this->func, *this->tup);
    }

    auto operator-> () -> ArrowProxy<decltype(**this)> {
      return {**this};
    }

    Iterator& operator++() {
      ++this->index;
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    bool operator!=(const Iterator& other) const {
      return this->index != other.index;
    }

    bool operator==(const Iterator& other) const {
      return !(*this != other);
    }
  };

  Iterator begin() {
    return {this->func, this->tup, 0};
  }

  Iterator end() {
    return {this->func, this->tup, sizeof...(Is)};
  }
};

template <typename Func, typename TupType, std::size_t... Is>
constexpr std::array<
    typename iter::impl::TupleStarMapper<Func, TupType, Is...>::CallerFunc,
    sizeof...(Is)> iter::impl::TupleStarMapper<Func, TupType, Is...>::callers;

template <typename Func, typename TupType, std::size_t... Is>
iter::impl::TupleStarMapper<Func, TupType, Is...>
iter::impl::starmap_with_tuples(
    Func func, TupType&& tup, std::index_sequence<Is...>) {
  return {std::move(func), std::forward<TupType>(tup)};
}

// handles tuple-like types
template <typename Func, typename TupType>
auto iter::impl::starmap_helper(Func func, TupType&& tup, std::true_type) {
  return starmap_with_tuples(std::move(func), std::forward<TupType>(tup),
      std::
          make_index_sequence<std::tuple_size<std::decay_t<TupType>>::value>{});
}

// handles everything else
template <typename Func, typename Container>
iter::impl::StarMapper<Func, Container> iter::impl::starmap_helper(
    Func func, Container&& container, std::false_type) {
  return {std::move(func), std::forward<Container>(container)};
}

namespace iter {
  namespace impl {
    template <typename T, typename = void>
    struct is_tuple_like : public std::false_type {};

    template <typename T>
    struct is_tuple_like<T,
        void_t<decltype(std::tuple_size<std::decay_t<T>>::value)>>
        : public std::true_type {};
  }
}

template <typename Func, typename Seq>
auto iter::starmap(Func func, Seq&& sequence) {
  return impl::starmap_helper(
      std::move(func), std::forward<Seq>(sequence), impl::is_tuple_like<Seq>{});
}

#endif
