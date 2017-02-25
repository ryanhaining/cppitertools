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

  using ProdIterDeref =
      std::tuple<iterator_deref<Container>, iterator_deref<RestContainers>...>;

 private:
  Container container_;
  Productor<RestContainers...> rest_products_;
  Productor(Container&& container, RestContainers&&... rest)
      : container_(std::forward<Container>(container)),
        rest_products_{std::forward<RestContainers>(rest)...} {}

 public:
  Productor(Productor&&) = default;
  class Iterator
      : public std::iterator<std::input_iterator_tag, ProdIterDeref> {
   private:
    using RestIter = typename Productor<RestContainers...>::Iterator;

    IteratorWrapper<Container> sub_iter_;
    IteratorWrapper<Container> sub_begin_;

    RestIter rest_iter_;
    RestIter rest_end_;

   public:
    constexpr static const bool is_base_iter = false;
    Iterator(IteratorWrapper<Container>&& sub_iter, RestIter&& rest_iter,
        RestIter&& rest_end)
        : sub_iter_{sub_iter},
          sub_begin_{sub_iter},
          rest_iter_{rest_iter},
          rest_end_{rest_end} {}

    void reset() {
      sub_iter_ = sub_begin_;
    }

    Iterator& operator++() {
      ++rest_iter_;
      if (!(rest_iter_ != rest_end_)) {
        rest_iter_.reset();
        ++sub_iter_;
      }
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    bool operator!=(const Iterator& other) const {
      return sub_iter_ != other.sub_iter_
             && (RestIter::is_base_iter || rest_iter_ != other.rest_iter_);
    }

    bool operator==(const Iterator& other) const {
      return !(*this != other);
    }

    ProdIterDeref operator*() {
      return std::tuple_cat(
          std::tuple<iterator_deref<Container>>{*sub_iter_}, *rest_iter_);
    }

    ArrowProxy<ProdIterDeref> operator->() {
      return {**this};
    }
  };

  Iterator begin() {
    return {get_begin(container_), get_begin(rest_products_),
        get_end(rest_products_)};
  }

  Iterator end() {
    return {
        get_end(container_), get_end(rest_products_), get_end(rest_products_)};
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
