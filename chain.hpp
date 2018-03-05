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

  template <typename TupTypeT>
  class IteratorData {
    IteratorData() = delete;
    static_assert(
        std::tuple_size<std::decay_t<TupTypeT>>::value == sizeof...(Is),
        "tuple size != sizeof Is");

    static_assert(
        are_same<iterator_deref<std::tuple_element_t<Is, TupTypeT>>...>::value,
        "All chained iterables must have iterators that "
        "dereference to the same type, including cv-qualifiers "
        "and references.");

   public:
    using IterTupType = iterator_tuple_type<TupTypeT>;
    using DerefType = iterator_deref<std::tuple_element_t<0, TupTypeT>>;
    using ArrowType = iterator_arrow<std::tuple_element_t<0, TupTypeT>>;

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

    using TraitsValue =
        iterator_traits_deref<std::tuple_element_t<0, TupTypeT>>;
  };

  Chained(TupType&& t) : tup_(std::move(t)) {}
  TupType tup_;

 public:
  Chained(Chained&&) = default;

  template <typename TupTypeT>
  class Iterator {
   private:
    using IterData = IteratorData<TupTypeT>;
    std::size_t index_;
    typename IterData::IterTupType iters_;
    typename IterData::IterTupType ends_;

    void check_for_end_and_adjust() {
      while (index_ < sizeof...(Is)
             && !(IterData::neq_comparers[index_](iters_, ends_))) {
        ++index_;
      }
    }

   public:
    using iterator_category = std::input_iterator_tag;
    using value_type = typename IteratorData<TupTypeT>::TraitsValue;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    Iterator(std::size_t i, typename IterData::IterTupType&& iters,
        typename IterData::IterTupType&& ends)
        : index_{i}, iters_(std::move(iters)), ends_(std::move(ends)) {
      check_for_end_and_adjust();
    }

    decltype(auto) operator*() {
      return IterData::derefers[index_](iters_);
    }

    decltype(auto) operator-> () {
      return IterData::arrowers[index_](iters_);
    }

    Iterator& operator++() {
      IterData::incrementers[index_](iters_);
      check_for_end_and_adjust();
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    // TODO make const and non-const iterators comparable
    bool operator!=(const Iterator& other) const {
      return index_ != other.index_
             || (index_ != sizeof...(Is)
                    && IterData::neq_comparers[index_](iters_, other.iters_));
    }

    bool operator==(const Iterator& other) const {
      return !(*this != other);
    }
  };

  Iterator<TupType> begin() {
    return {0, typename IteratorData<TupType>::IterTupType{get_begin(
                   std::get<Is>(tup_))...},
        typename IteratorData<TupType>::IterTupType{
            get_end(std::get<Is>(tup_))...}};
  }

  Iterator<TupType> end() {
    return {sizeof...(Is), typename IteratorData<TupType>::IterTupType{get_end(
                               std::get<Is>(tup_))...},
        typename IteratorData<TupType>::IterTupType{
            get_end(std::get<Is>(tup_))...}};
  }

  Iterator<AsConst<TupType>> begin() const {
    return {0, typename IteratorData<AsConst<TupType>>::IterTupType{get_begin(
                   impl::as_const(std::get<Is>(tup_)))...},
        typename IteratorData<AsConst<TupType>>::IterTupType{
            get_end(impl::as_const(std::get<Is>(tup_)))...}};
  }

  Iterator<AsConst<TupType>> end() const {
    return {sizeof...(Is),
        typename IteratorData<AsConst<TupType>>::IterTupType{
            get_end(impl::as_const(std::get<Is>(tup_)))...},
        typename IteratorData<AsConst<TupType>>::IterTupType{
            get_end(impl::as_const(std::get<Is>(tup_)))...}};
  }
};

// jesus christ. what have I done.
template <typename TupType, std::size_t... Is>
template <typename TupTypeT>
constexpr std::array<typename iter::impl::Chained<TupType,
                         Is...>::template IteratorData<TupTypeT>::DerefFunc,
    sizeof...(Is)>
    iter::impl::Chained<TupType, Is...>::IteratorData<TupTypeT>::derefers;

template <typename TupType, std::size_t... Is>
template <typename TupTypeT>
constexpr std::array<typename iter::impl::Chained<TupType,
                         Is...>::template IteratorData<TupTypeT>::ArrowFunc,
    sizeof...(Is)>
    iter::impl::Chained<TupType, Is...>::IteratorData<TupTypeT>::arrowers;

template <typename TupType, std::size_t... Is>
template <typename TupTypeT>
constexpr std::array<typename iter::impl::Chained<TupType,
                         Is...>::template IteratorData<TupTypeT>::IncFunc,
    sizeof...(Is)>
    iter::impl::Chained<TupType, Is...>::IteratorData<TupTypeT>::incrementers;

template <typename TupType, std::size_t... Is>
template <typename TupTypeT>
constexpr std::array<typename iter::impl::Chained<TupType,
                         Is...>::template IteratorData<TupTypeT>::NeqFunc,
    sizeof...(Is)>
    iter::impl::Chained<TupType, Is...>::IteratorData<TupTypeT>::neq_comparers;

template <typename Container>
class iter::impl::ChainedFromIterable {
 private:
  friend ChainFromIterableFn;
  Container container_;
  ChainedFromIterable(Container&& container)
      : container_(std::forward<Container>(container)) {}

 public:
  ChainedFromIterable(ChainedFromIterable&&) = default;
  template <typename ContainerT>
  class Iterator {
   private:
    template <typename>
    friend class Iterator;
    using SubContainer = iterator_deref<ContainerT>;
    using SubIter = IteratorWrapper<SubContainer>;

    IteratorWrapper<ContainerT> top_level_iter_;
    IteratorWrapper<ContainerT> top_level_end_;
    std::unique_ptr<SubIter> sub_iter_p_;
    std::unique_ptr<SubIter> sub_end_p_;

    static std::unique_ptr<SubIter> clone_sub_pointer(const SubIter* sub_iter) {
      return sub_iter ? std::make_unique<SubIter>(*sub_iter) : nullptr;
    }

    template <typename T>
    bool sub_iters_differ(const Iterator<T>& other) const {
      // checking if they're the same also handles them both being nullptr
      if (static_cast<const void*>(sub_iter_p_.get())
          == static_cast<const void*>(other.sub_iter_p_.get())) {
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
    using iterator_category = std::input_iterator_tag;
    using value_type = iterator_traits_deref<iterator_deref<ContainerT>>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    Iterator(IteratorWrapper<ContainerT>&& top_iter,
        IteratorWrapper<ContainerT>&& top_end)
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

    template <typename T>
    bool operator!=(const Iterator<T>& other) const {
      return top_level_iter_ != other.top_level_iter_
             || sub_iters_differ(other);
    }

    template <typename T>
    bool operator==(const Iterator<T>& other) const {
      return !(*this != other);
    }

    iterator_deref<iterator_deref<ContainerT>> operator*() {
      return **sub_iter_p_;
    }

    iterator_arrow<iterator_deref<ContainerT>> operator->() {
      return apply_arrow(*sub_iter_p_);
    }
  };

  Iterator<Container> begin() {
    return {get_begin(container_), get_end(container_)};
  }

  Iterator<Container> end() {
    return {get_end(container_), get_end(container_)};
  }

  Iterator<AsConst<Container>> begin() const {
    return {get_begin(impl::as_const(container_)),
        get_end(impl::as_const(container_))};
  }

  Iterator<AsConst<Container>> end() const {
    return {get_end(impl::as_const(container_)),
        get_end(impl::as_const(container_))};
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
