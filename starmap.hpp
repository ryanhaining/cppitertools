#ifndef ITER_STARMAP_H_
#define ITER_STARMAP_H_

#include "internal/iter_tuples.hpp"
#include "internal/iterator_wrapper.hpp"
#include "internal/iterbase.hpp"

#include <array>
#include <cassert>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

namespace iter {
  namespace impl {
    template <typename Func, typename Container>
    class StarMapper;

    template <typename Func, typename TupType, std::size_t... Is>
    class TupleStarMapper;

    struct StarMapFn;
  }
}

// NOTE I don't know why, but clang gets very confused by having  in the
// Iterators' member functions for these classes

// starmap with a container_<T> where T is one of tuple, pair, array
template <typename Func, typename Container>
class iter::impl::StarMapper {
 private:
  Func func_;
  Container container_;

  using StarIterDeref = std::remove_reference_t<decltype(
      call_with_tuple(func_, std::declval<iterator_deref<Container>>()))>;

  StarMapper(Func f, Container&& c)
      : func_(std::move(f)), container_(std::forward<Container>(c)) {}

  friend StarMapFn;

 public:
  class Iterator
      : public std::iterator<std::input_iterator_tag, StarIterDeref> {
   private:
    Func* func_;
    IteratorWrapper<Container> sub_iter_;

   public:
    Iterator(Func& f, IteratorWrapper<Container>&& sub_iter)
        : func_(&f), sub_iter_(std::move(sub_iter)) {}

    bool operator!=(const Iterator& other) const {
      return sub_iter_ != other.sub_iter_;
    }

    bool operator==(const Iterator& other) const {
      return !(*this != other);
    }

    Iterator& operator++() {
      ++sub_iter_;
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    decltype(auto) operator*() {
      return call_with_tuple(*func_, *sub_iter_);
    }

    auto operator-> () -> ArrowProxy<decltype(**this)> {
      return {**this};
    }
  };

  Iterator begin() {
    return {func_, get_begin(container_)};
  }

  Iterator end() {
    return {func_, get_end(container_)};
  }
};

// starmap for a tuple or pair of tuples or pairs
template <typename Func, typename TupType, std::size_t... Is>
class iter::impl::TupleStarMapper {
 private:
  Func func_;
  TupType tup_;

 private:
  static_assert(sizeof...(Is) == std::tuple_size<std::decay_t<TupType>>::value,
      "tuple size doesn't match size of Is");

  friend StarMapFn;

  template <std::size_t Idx>
  static decltype(auto) get_and_call_with_tuple(Func& f, TupType& t) {
    return call_with_tuple(f, std::get<Idx>(t));
  }

  using ResultType = decltype(get_and_call_with_tuple<0>(func_, tup_));
  using CallerFunc = ResultType (*)(Func&, TupType&);

  constexpr static std::array<CallerFunc, sizeof...(Is)> callers{
      {get_and_call_with_tuple<Is>...}};

  using TraitsValue = std::remove_reference_t<ResultType>;

  TupleStarMapper(Func f, TupType t)
      : func_(std::move(f)), tup_(std::forward<TupType>(t)) {}

 public:
  class Iterator : public std::iterator<std::input_iterator_tag, TraitsValue> {
   private:
    Func* func_;
    std::remove_reference_t<TupType>* tup_;
    std::size_t index_;

   public:
    Iterator(Func& f, TupType& t, std::size_t i)
        : func_{&f}, tup_{&t}, index_{i} {}

    decltype(auto) operator*() {
      return callers[index_](*func_, *tup_);
    }

    auto operator-> () -> ArrowProxy<decltype(**this)> {
      return {**this};
    }

    Iterator& operator++() {
      ++index_;
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    bool operator!=(const Iterator& other) const {
      return index_ != other.index_;
    }

    bool operator==(const Iterator& other) const {
      return !(*this != other);
    }
  };

  Iterator begin() {
    return {func_, tup_, 0};
  }

  Iterator end() {
    return {func_, tup_, sizeof...(Is)};
  }
};

template <typename Func, typename TupType, std::size_t... Is>
constexpr std::array<
    typename iter::impl::TupleStarMapper<Func, TupType, Is...>::CallerFunc,
    sizeof...(Is)>
    iter::impl::TupleStarMapper<Func, TupType, Is...>::callers;

struct iter::impl::StarMapFn : PipeableAndBindFirst<StarMapFn> {
 private:
  template <typename Func, typename TupType, std::size_t... Is>
  TupleStarMapper<Func, TupType, Is...> helper_with_tuples(
      Func func, TupType&& tup, std::index_sequence<Is...>) const {
    return {std::move(func), std::forward<TupType>(tup)};
  }

  // handles tuple-like types
  template <typename Func, typename TupType>
  auto helper(Func func, TupType&& tup, std::true_type) const {
    return helper_with_tuples(std::move(func), std::forward<TupType>(tup),
        std::make_index_sequence<
            std::tuple_size<std::decay_t<TupType>>::value>{});
  }

  // handles everything else
  template <typename Func, typename Container>
  StarMapper<Func, Container> helper(
      Func func, Container&& container, std::false_type) const {
    return {std::move(func), std::forward<Container>(container)};
  }

  template <typename T, typename = void>
  struct is_tuple_like : public std::false_type {};

  template <typename T>
  struct is_tuple_like<T,
      void_t<decltype(std::tuple_size<std::decay_t<T>>::value)>>
      : public std::true_type {};

 public:
  template <typename Func, typename Seq>
  auto operator()(Func func, Seq&& sequence) const {
    return helper(
        std::move(func), std::forward<Seq>(sequence), is_tuple_like<Seq>{});
  }

  using PipeableAndBindFirst<StarMapFn>::operator();
};

namespace iter {
  constexpr impl::StarMapFn starmap{};
}

#endif
