#ifndef ITER_PRODUCT_HPP_
#define ITER_PRODUCT_HPP_

#include "internal/iterator_wrapper.hpp"
#include "internal/iterbase.hpp"

#include <array>
#include <iterator>
#include <tuple>
#include <utility>

namespace iter {
  namespace impl {
    template <typename... Containers>
    class Productor;

    template <typename Container, typename... RestContainers>
    class Productor<Container, RestContainers...>;

    template <>
    class Productor<>;
  }

  template <typename... Containers>
  impl::Productor<Containers...> product(Containers&&...);
}

// specialization for at least 1 template argument
template <typename Container, typename... RestContainers>
class iter::impl::Productor<Container, RestContainers...> {
  friend Productor iter::product<Container, RestContainers...>(
      Container&&, RestContainers&&...);

  template <typename... RC>
  friend class Productor;

  template <typename T>
  using ProdIterDeref =
      std::tuple<iterator_deref<T>, iterator_deref<RestContainers>...>;

 private:
  Container container_;
  Productor<RestContainers...> rest_products_;
  Productor(Container&& container, RestContainers&&... rest)
      : container_(std::forward<Container>(container)),
        rest_products_{std::forward<RestContainers>(rest)...} {}

 public:
  Productor(Productor&&) = default;

 private:
  template <typename ContainerT, typename RestIter>
  class IteratorTempl {
   private:
    template <typename, typename>
    friend class IteratorTempl;
    IteratorWrapper<ContainerT> sub_iter_;
    IteratorWrapper<ContainerT> sub_begin_;

    RestIter rest_iter_;
    RestIter rest_end_;

   public:
    using iterator_category = std::input_iterator_tag;
    using value_type = ProdIterDeref<ContainerT>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    constexpr static const bool is_base_iter = false;
    IteratorTempl(IteratorWrapper<ContainerT>&& sub_iter, RestIter&& rest_iter,
        RestIter&& rest_end)
        : sub_iter_{sub_iter},
          sub_begin_{sub_iter},
          rest_iter_{rest_iter},
          rest_end_{rest_end} {}

    void reset() {
      sub_iter_ = sub_begin_;
    }

    IteratorTempl& operator++() {
      ++rest_iter_;
      if (!(rest_iter_ != rest_end_)) {
        rest_iter_.reset();
        ++sub_iter_;
      }
      return *this;
    }

    IteratorTempl operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    template <typename T, typename U>
    bool operator!=(const IteratorTempl<T, U>& other) const {
      return sub_iter_ != other.sub_iter_
             && (RestIter::is_base_iter || rest_iter_ != other.rest_iter_);
    }

    template <typename T, typename U>
    bool operator==(const IteratorTempl<T, U>& other) const {
      return !(*this != other);
    }

    ProdIterDeref<ContainerT> operator*() {
      return std::tuple_cat(
          std::tuple<iterator_deref<ContainerT>>{*sub_iter_}, *rest_iter_);
    }

    ArrowProxy<ProdIterDeref<ContainerT>> operator->() {
      return {**this};
    }
  };

  using RestIter = typename Productor<RestContainers...>::Iterator;
  using RestConstIter = typename Productor<RestContainers...>::ConstIterator;

 public:
  using Iterator = IteratorTempl<Container, RestIter>;
  using ConstIterator = IteratorTempl<AsConst<Container>, RestConstIter>;

  Iterator begin() {
    return {get_begin(container_), get_begin(rest_products_),
        get_end(rest_products_)};
  }

  Iterator end() {
    return {
        get_end(container_), get_end(rest_products_), get_end(rest_products_)};
  }

  ConstIterator begin() const {
    return {get_begin(impl::as_const(container_)),
        get_begin(impl::as_const(rest_products_)),
        get_end(impl::as_const(rest_products_))};
  }

  ConstIterator end() const {
    return {get_end(impl::as_const(container_)),
        get_end(impl::as_const(rest_products_)),
        get_end(impl::as_const(rest_products_))};
  }
};

template <>
class iter::impl::Productor<> {
 public:
  Productor(Productor&&) = default;
  class Iterator {
   public:
    using iterator_category = std::input_iterator_tag;
    using value_type = std::tuple<>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    constexpr static const bool is_base_iter = true;

    void reset() {}

    Iterator& operator++() {
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    // see note in zip about base case operator!=
    bool operator!=(const Iterator&) const {
      return false;
    }

    bool operator==(const Iterator& other) const {
      return !(*this != other);
    }

    std::tuple<> operator*() const {
      return {};
    }
  };
  using ConstIterator = Iterator;

  Iterator begin() {
    return {};
  }

  Iterator end() {
    return {};
  }

  ConstIterator begin() const {
    return {};
  }

  ConstIterator end() const {
    return {};
  }
};

template <typename... Containers>
iter::impl::Productor<Containers...> iter::product(Containers&&... containers) {
  return {std::forward<Containers>(containers)...};
}

namespace iter {
  namespace impl {
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
  template <std::size_t N, typename Container>
  decltype(auto) product(Container&& container) {
    return impl::product_repeat(
        std::make_index_sequence<N>{}, std::forward<Container>(container));
  }

  constexpr std::array<std::tuple<>, 1> product() {
    return {{}};
  }
}

#endif
