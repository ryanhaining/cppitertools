#ifndef ITER_CHAIN_HPP_
#define ITER_CHAIN_HPP_

#include "internal/iter_tuples.hpp"
#include "internal/iterator_wrapper.hpp"
#include "internal/iterbase.hpp"

#include <array>
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

    using ChainFromIterableFn = IterToolFn<ChainedFromIterable>;

    // rather than a chain function, use a callable object to support
    // from_iterable
    class ChainMaker;
  }
}

template <typename TupType, std::size_t... Is>
class iter::impl::Chained {
 private:
  friend ChainMaker;

  static_assert(std::tuple_size<std::decay_t<TupType>>::value == sizeof...(Is),
      "tuple size != sizeof Is");

  static_assert(
      are_same<iterator_deref<std::tuple_element_t<Is, TupType>>...>::value,
      "All chained iterables must have iterators that "
      "dereference to the same type, including cv-qualifiers "
      "and references.");

  using IterTupType = iterator_tuple_type<TupType>;
  using DerefType = iterator_deref<std::tuple_element_t<0, TupType>>;
  using ArrowType = iterator_arrow<std::tuple_element_t<0, TupType>>;

  template <std::size_t Idx>
  static DerefType get_and_deref(IterTupType& iters) {
    return *std::get<Idx>(iters);
  }

  template <std::size_t Idx>
  static ArrowType get_and_arrow(IterTupType& iters) {
    return apply_arrow(std::get<Idx>(iters));
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
  using ArrowFunc = ArrowType (*)(IterTupType&);
  using IncFunc = void (*)(IterTupType&);
  using NeqFunc = bool (*)(const IterTupType&, const IterTupType&);

  constexpr static std::array<DerefFunc, sizeof...(Is)> derefers{
      {get_and_deref<Is>...}};

  constexpr static std::array<ArrowFunc, sizeof...(Is)> arrowers{
      {get_and_arrow<Is>...}};

  constexpr static std::array<IncFunc, sizeof...(Is)> incrementers{
      {get_and_increment<Is>...}};

  constexpr static std::array<NeqFunc, sizeof...(Is)> neq_comparers{
      {get_and_check_not_equal<Is>...}};

  using TraitsValue = iterator_traits_deref<std::tuple_element_t<0, TupType>>;

 private:
  Chained(TupType&& t) : tup_(std::move(t)) {}
  TupType tup_;

 public:
  Chained(Chained&&) = default;

  class Iterator : public std::iterator<std::input_iterator_tag, TraitsValue> {
   private:
    std::size_t index_;
    IterTupType iters_;
    IterTupType ends_;

    void check_for_end_and_adjust() {
      while (
          index_ < sizeof...(Is) && !(neq_comparers[index_](iters_, ends_))) {
        ++index_;
      }
    }

   public:
    Iterator(std::size_t i, IterTupType&& iters, IterTupType&& ends)
        : index_{i}, iters_(std::move(iters)), ends_(std::move(ends)) {
      check_for_end_and_adjust();
    }

    decltype(auto) operator*() {
      return derefers[index_](iters_);
    }

    decltype(auto) operator-> () {
      return arrowers[index_](iters_);
    }

    Iterator& operator++() {
      incrementers[index_](iters_);
      check_for_end_and_adjust();
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    bool operator!=(const Iterator& other) const {
      return index_ != other.index_
             || (index_ != sizeof...(Is)
                    && neq_comparers[index_](iters_, other.iters_));
    }

    bool operator==(const Iterator& other) const {
      return !(*this != other);
    }
  };

  Iterator begin() {
    return {0, IterTupType{get_begin(std::get<Is>(tup_))...},
        IterTupType{get_end(std::get<Is>(tup_))...}};
  }

  Iterator end() {
    return {sizeof...(Is), IterTupType{get_end(std::get<Is>(tup_))...},
        IterTupType{get_end(std::get<Is>(tup_))...}};
  }
};

template <typename TupType, std::size_t... Is>
constexpr std::array<typename iter::impl::Chained<TupType, Is...>::DerefFunc,
    sizeof...(Is)>
    iter::impl::Chained<TupType, Is...>::derefers;

template <typename TupType, std::size_t... Is>
constexpr std::array<typename iter::impl::Chained<TupType, Is...>::ArrowFunc,
    sizeof...(Is)>
    iter::impl::Chained<TupType, Is...>::arrowers;

template <typename TupType, std::size_t... Is>
constexpr std::array<typename iter::impl::Chained<TupType, Is...>::IncFunc,
    sizeof...(Is)>
    iter::impl::Chained<TupType, Is...>::incrementers;

template <typename TupType, std::size_t... Is>
constexpr std::array<typename iter::impl::Chained<TupType, Is...>::NeqFunc,
    sizeof...(Is)>
    iter::impl::Chained<TupType, Is...>::neq_comparers;

template <typename Container>
class iter::impl::ChainedFromIterable {
 private:
  friend ChainFromIterableFn;
  Container container_;
  ChainedFromIterable(Container&& container)
      : container_(std::forward<Container>(container)) {}

 public:
  ChainedFromIterable(ChainedFromIterable&&) = default;
  class Iterator : public std::iterator<std::input_iterator_tag,
                       iterator_traits_deref<iterator_deref<Container>>> {
   private:
    using SubContainer = iterator_deref<Container>;
    using SubIter = IteratorWrapper<SubContainer>;

    IteratorWrapper<Container> top_level_iter_;
    IteratorWrapper<Container> top_level_end_;
    std::unique_ptr<SubIter> sub_iter_p_;
    std::unique_ptr<SubIter> sub_end_p_;

    static std::unique_ptr<SubIter> clone_sub_pointer(const SubIter* sub_iter) {
      return sub_iter ? std::make_unique<SubIter>(*sub_iter) : nullptr;
    }

    bool sub_iters_differ(const Iterator& other) const {
      if (sub_iter_p_ == other.sub_iter_p_) {
        return false;
      }
      if (sub_iter_p_ == nullptr || other.sub_iter_p_ == nullptr) {
        // since the first check tests if they're the same,
        // this will return if only one is nullptr
        return true;
      }
      return *sub_iter_p_ != *other.sub_iter_p_;
    }

   public:
    Iterator(IteratorWrapper<Container>&& top_iter,
        IteratorWrapper<Container>&& top_end)
        : top_level_iter_{std::move(top_iter)},
          top_level_end_{std::move(top_end)},
          sub_iter_p_{!(top_iter != top_end)
                          ?  // iter == end ?
                          nullptr
                          : std::make_unique<SubIter>(get_begin(*top_iter))},
          sub_end_p_{!(top_iter != top_end)
                         ?  // iter == end ?
                         nullptr
                         : std::make_unique<SubIter>(get_end(*top_iter))} {}

    Iterator(const Iterator& other)
        : top_level_iter_{other.top_level_iter_},
          top_level_end_{other.top_level_end_},
          sub_iter_p_{clone_sub_pointer(other.sub_iter_p_.get())},
          sub_end_p_{clone_sub_pointer(other.sub_end_p_.get())} {}

    Iterator& operator=(const Iterator& other) {
      if (this == &other) {
        return *this;
      }

      top_level_iter_ = other.top_level_iter_;
      top_level_end_ = other.top_level_end_;
      sub_iter_p_ = clone_sub_pointer(other.sub_iter_p_.get());
      sub_end_p_ = clone_sub_pointer(other.sub_end_p_.get());

      return *this;
    }

    Iterator(Iterator&&) = default;
    Iterator& operator=(Iterator&&) = default;
    ~Iterator() = default;

    Iterator& operator++() {
      ++*sub_iter_p_;
      if (!(*sub_iter_p_ != *sub_end_p_)) {
        ++top_level_iter_;
        if (top_level_iter_ != top_level_end_) {
          sub_iter_p_ = std::make_unique<SubIter>(get_begin(*top_level_iter_));
          sub_end_p_ = std::make_unique<SubIter>(get_end(*top_level_iter_));
        } else {
          sub_iter_p_.reset();
          sub_end_p_.reset();
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
      return top_level_iter_ != other.top_level_iter_
             || sub_iters_differ(other);
    }

    bool operator==(const Iterator& other) const {
      return !(*this != other);
    }

    iterator_deref<iterator_deref<Container>> operator*() {
      return **sub_iter_p_;
    }

    iterator_arrow<iterator_deref<Container>> operator->() {
      return apply_arrow(*sub_iter_p_);
    }
  };

  Iterator begin() {
    return {get_begin(container_), get_end(container_)};
  }

  Iterator end() {
    return {get_end(container_), get_end(container_)};
  }
};

class iter::impl::ChainMaker {
 private:
  template <typename TupleType, std::size_t... Is>
  Chained<TupleType, Is...> chain_impl(
      TupleType&& containers, std::index_sequence<Is...>) const {
    return {std::move(containers)};
  }

 public:
  // expose regular call operator to provide usual chain()
  template <typename... Containers>
  auto operator()(Containers&&... cs) const {
    return chain_impl(
        std::tuple<Containers...>{std::forward<Containers>(cs)...},
        std::index_sequence_for<Containers...>{});
  }

  ChainFromIterableFn from_iterable;
};

namespace iter {
  namespace {
    constexpr auto chain = iter::impl::ChainMaker{};
  }
}

#endif
