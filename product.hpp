#ifndef ITER_PRODUCT_HPP_
#define ITER_PRODUCT_HPP_

#include "internal/iterbase.hpp"

#include <iterator>
#include <tuple>
#include <utility>
#include <array>

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

  using ProdIterDeref =
      std::tuple<iterator_deref<Container>, iterator_deref<RestContainers>...>;

 private:
  Container container;
  Productor<RestContainers...> rest_products;
  Productor(Container&& in_container, RestContainers&&... rest)
      : container(std::forward<Container>(in_container)),
        rest_products{std::forward<RestContainers>(rest)...} {}

 public:
  Productor(Productor&&) = default;
  class Iterator
      : public std::iterator<std::input_iterator_tag, ProdIterDeref> {
   private:
    using RestIter = typename Productor<RestContainers...>::Iterator;

    iterator_type<Container> iter;
    iterator_type<Container> begin;

    RestIter rest_iter;
    RestIter rest_end;

   public:
    constexpr static const bool is_base_iter = false;
    Iterator(const iterator_type<Container>& it, RestIter&& rest,
        RestIter&& in_rest_end)
        : iter{it}, begin{it}, rest_iter{rest}, rest_end{in_rest_end} {}

    void reset() {
      this->iter = this->begin;
    }

    Iterator& operator++() {
      ++this->rest_iter;
      if (!(this->rest_iter != this->rest_end)) {
        this->rest_iter.reset();
        ++this->iter;
      }
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    bool operator!=(const Iterator& other) const {
      return this->iter != other.iter
             && (RestIter::is_base_iter || this->rest_iter != other.rest_iter);
    }

    bool operator==(const Iterator& other) const {
      return !(*this != other);
    }

    ProdIterDeref operator*() {
      return std::tuple_cat(
          std::tuple<iterator_deref<Container>>{*this->iter}, *this->rest_iter);
    }

    ArrowProxy<ProdIterDeref> operator->() {
      return {**this};
    }
  };

  Iterator begin() {
    return {std::begin(this->container), std::begin(this->rest_products),
        std::end(this->rest_products)};
  }

  Iterator end() {
    return {std::end(this->container), std::end(this->rest_products),
        std::end(this->rest_products)};
  }
};

template <>
class iter::impl::Productor<> {
 public:
  Productor(Productor&&) = default;
  class Iterator : public std::iterator<std::input_iterator_tag, std::tuple<>> {
   public:
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

  Iterator begin() {
    return {};
  }

  Iterator end() {
    return {};
  }
};

template <typename... Containers>
iter::impl::Productor<Containers...> iter::product(Containers&&... containers) {
  return {std::forward<Containers>(containers)...};
}

namespace iter {
  constexpr std::array<std::tuple<>, 1> product() {
    return {{}};
  }
}

#endif
