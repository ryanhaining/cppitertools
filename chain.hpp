#ifndef ITER_CHAIN_HPP_
#define ITER_CHAIN_HPP_

#include "internal/iterbase.hpp"

#include <array>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>

namespace iter {
  namespace impl {
    template <typename TupType, std::size_t... Is>
    class Chained;

    template <typename Container>
    class ChainedFromIterable;

    // rather than a chain function, use a callable object to support
    // from_iterable
    class ChainMaker;
  }
}

template <typename TupType, std::size_t... Is>
class iter::impl::Chained {
 private:
  friend class ChainMaker;

  static_assert(std::tuple_size<std::decay_t<TupType>>::value == sizeof...(Is),
      "tuple size != sizeof Is");

  static_assert(
      are_same<iterator_deref<std::tuple_element_t<Is, TupType>>...>::value,
      "All chained iterables must have iterators that "
      "dereference to the same type, including cv-qualifiers "
      "and references.");

  using IterTupType = iterator_tuple_type<TupType>;
  using DerefType = iterator_deref<std::tuple_element_t<0, TupType>>;

  template <std::size_t Idx>
  static DerefType get_and_deref(IterTupType& iters) {
    return *std::get<Idx>(iters);
  }

  template <std::size_t Idx>
  static void get_and_increment(IterTupType& iters) {
    ++std::get<Idx>(iters);
  }

  template <std::size_t Idx>
  static bool get_and_check_not_equal(
      const IterTupType& lhs, const IterTupType& rhs) {
    return std::get<Idx>(lhs) != std::get<Idx>(rhs);
  }

  using DerefFunc = DerefType (*)(IterTupType&);
  using IncFunc = void (*)(IterTupType&);
  using NeqFunc = bool (*)(const IterTupType&, const IterTupType&);

  constexpr static std::array<DerefFunc, sizeof...(Is)> derefers{
      {get_and_deref<Is>...}};

  constexpr static std::array<IncFunc, sizeof...(Is)> incrementers{
      {get_and_increment<Is>...}};

  constexpr static std::array<NeqFunc, sizeof...(Is)> neq_comparers{
      {get_and_check_not_equal<Is>...}};

  using TraitsValue = iterator_traits_deref<std::tuple_element_t<0, TupType>>;

 private:
  TupType tup;

 public:
  Chained(Chained&&) = default;
  Chained(TupType&& t) : tup(std::move(t)) {}

  class Iterator : public std::iterator<std::input_iterator_tag, TraitsValue> {
   private:
    std::size_t index;
    IterTupType iters;
    IterTupType ends;

    void check_for_end_and_adjust() {
      while (this->index < sizeof...(Is)
             && !(neq_comparers[this->index](this->iters, this->ends))) {
        ++this->index;
      }
    }

   public:
    Iterator(std::size_t i, IterTupType&& in_iters, IterTupType&& in_ends)
        : index{i}, iters(in_iters), ends(in_ends) {
      this->check_for_end_and_adjust();
    }

    decltype(auto) operator*() {
      return derefers[this->index](this->iters);
    }

    Iterator& operator++() {
      incrementers[this->index](this->iters);
      this->check_for_end_and_adjust();
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    bool operator!=(const Iterator& other) const {
      return this->index != other.index
             || (this->index != sizeof...(Is)
                    && neq_comparers[this->index](this->iters, other.iters));
    }

    bool operator==(const Iterator& other) const {
      return !(*this != other);
    }
  };

  Iterator begin() {
    return {0, IterTupType{std::begin(std::get<Is>(this->tup))...},
        IterTupType{std::end(std::get<Is>(this->tup))...}};
  }

  Iterator end() {
    return {sizeof...(Is), IterTupType{std::end(std::get<Is>(this->tup))...},
        IterTupType{std::end(std::get<Is>(this->tup))...}};
  }
};

template <typename TupType, std::size_t... Is>
constexpr std::array<typename iter::impl::Chained<TupType, Is...>::DerefFunc,
    sizeof...(Is)> iter::impl::Chained<TupType, Is...>::derefers;

template <typename TupType, std::size_t... Is>
constexpr std::array<typename iter::impl::Chained<TupType, Is...>::IncFunc,
    sizeof...(Is)> iter::impl::Chained<TupType, Is...>::incrementers;

template <typename TupType, std::size_t... Is>
constexpr std::array<typename iter::impl::Chained<TupType, Is...>::NeqFunc,
    sizeof...(Is)> iter::impl::Chained<TupType, Is...>::neq_comparers;

template <typename Container>
class iter::impl::ChainedFromIterable {
 private:
  Container container;
  friend class ChainMaker;
  ChainedFromIterable(Container&& in_container)
      : container(std::forward<Container>(in_container)) {}

 public:
  class Iterator : public std::iterator<std::input_iterator_tag,
                       iterator_traits_deref<iterator_deref<Container>>> {
   private:
    using SubContainer = iterator_deref<Container>;
    using SubIter = iterator_type<SubContainer>;

    iterator_type<Container> top_level_iter;
    iterator_type<Container> top_level_end;
    std::unique_ptr<SubIter> sub_iter_p;
    std::unique_ptr<SubIter> sub_end_p;

    static std::unique_ptr<SubIter> clone_sub_pointer(const SubIter* sub_iter) {
      return sub_iter ? std::make_unique<SubIter>(*sub_iter) : nullptr;
    }

    bool sub_iters_differ(const Iterator& other) const {
      if (this->sub_iter_p == other.sub_iter_p) {
        return false;
      }
      if (this->sub_iter_p == nullptr || other.sub_iter_p == nullptr) {
        // since the first check tests if they're the same,
        // this will return if only one is nullptr
        return true;
      }
      return *this->sub_iter_p != *other.sub_iter_p;
    }

   public:
    Iterator(
        iterator_type<Container>&& top_iter, iterator_type<Container>&& top_end)
        : top_level_iter{std::move(top_iter)},
          top_level_end{std::move(top_end)},
          sub_iter_p{!(top_iter != top_end)
                         ?  // iter == end ?
                         nullptr
                         : std::make_unique<SubIter>(std::begin(*top_iter))},
          sub_end_p{!(top_iter != top_end)
                        ?  // iter == end ?
                        nullptr
                        : std::make_unique<SubIter>(std::end(*top_iter))} {}

    Iterator(const Iterator& other)
        : top_level_iter{other.top_level_iter},
          top_level_end{other.top_level_end},
          sub_iter_p{clone_sub_pointer(other.sub_iter_p.get())},
          sub_end_p{clone_sub_pointer(other.sub_end_p.get())} {}

    Iterator& operator=(const Iterator& other) {
      if (this == &other) return *this;

      this->top_level_iter = other.top_level_iter;
      this->top_level_end = other.top_level_end;
      this->sub_iter_p = clone_sub_pointer(other.sub_iter_p.get());
      this->sub_end_p = clone_sub_pointer(other.sub_end_p.get());

      return *this;
    }

    Iterator(Iterator&&) = default;
    Iterator& operator=(Iterator&&) = default;
    ~Iterator() = default;

    Iterator& operator++() {
      ++*this->sub_iter_p;
      if (!(*this->sub_iter_p != *this->sub_end_p)) {
        ++this->top_level_iter;
        if (this->top_level_iter != this->top_level_end) {
          sub_iter_p =
              std::make_unique<SubIter>(std::begin(*this->top_level_iter));
          sub_end_p =
              std::make_unique<SubIter>(std::end(*this->top_level_iter));
        } else {
          sub_iter_p.reset();
          sub_end_p.reset();
        }
      }
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    bool operator!=(const Iterator& other) const {
      return this->top_level_iter != other.top_level_iter
             || this->sub_iters_differ(other);
    }

    bool operator==(const Iterator& other) const {
      return !(*this != other);
    }

    iterator_deref<iterator_deref<Container>> operator*() {
      return **this->sub_iter_p;
    }
  };

  Iterator begin() {
    return {std::begin(this->container), std::end(this->container)};
  }

  Iterator end() {
    return {std::end(this->container), std::end(this->container)};
  }
};

class iter::impl::ChainMaker {
 private:
  template <typename TupleType, std::size_t... Is>
  Chained<TupleType, Is...> chain_impl(
      TupleType&& in_containers, std::index_sequence<Is...>) const {
    return {std::move(in_containers)};
  }

 public:
  // expose regular call operator to provide usual chain()
  template <typename... Containers>
  auto operator()(Containers&&... cs) const {
    return this->chain_impl(
        std::tuple<Containers...>{std::forward<Containers>(cs)...},
        std::index_sequence_for<Containers...>{});
  }

  // chain.from_iterable
  template <typename Container>
  ChainedFromIterable<Container> from_iterable(Container&& container) const {
    return {std::forward<Container>(container)};
  }
};

namespace iter {
  namespace {
    constexpr auto chain = iter::impl::ChainMaker{};
  }
}

#endif
