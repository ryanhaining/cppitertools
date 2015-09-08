#ifndef ITER_CHAIN_HPP_
#define ITER_CHAIN_HPP_

#include "internal/iterbase.hpp"

#include <utility>
#include <iterator>
#include <memory>
#include <initializer_list>
#include <type_traits>

namespace iter {
  namespace impl {
    template <typename Container, typename... RestContainers>
    class Chained;

    template <typename Container>
    class Chained<Container>;

    template <typename Container>
    class ChainedFromIterable;

    // rather than a chain function, use a callable object to support
    // .from_iterable()
    class ChainMaker;
  }
}

template <typename Container, typename... RestContainers>
class iter::impl::Chained {
  static_assert(are_same<iterator_deref<Container>,
                    iterator_deref<RestContainers>...>::value,
      "All chained iterables must have iterators that "
      "dereference to the same type, including cv-qualifiers "
      "and references.");

  friend class ChainMaker;
  template <typename, typename...>
  friend class Chained;

 private:
  Container container;
  Chained<RestContainers...> rest_chained;
  Chained(Container&& in_container, RestContainers&&... rest)
      : container(std::forward<Container>(in_container)),
        rest_chained{std::forward<RestContainers>(rest)...} {}

 public:
  Chained(Chained&&) = default;
  class Iterator : public std::iterator<std::input_iterator_tag,
                       iterator_traits_deref<Container>> {
   private:
    using RestIter = typename Chained<RestContainers...>::Iterator;
    iterator_type<Container> sub_iter;
    iterator_type<Container> sub_end;
    RestIter rest_iter;
    bool at_end;

   public:
    Iterator(iterator_type<Container>&& s_begin,
        iterator_type<Container>&& s_end, RestIter&& in_rest_iter)
        : sub_iter{std::move(s_begin)},
          sub_end{std::move(s_end)},
          rest_iter{std::move(in_rest_iter)},
          at_end{!(sub_iter != sub_end)} {}

    Iterator& operator++() {
      if (this->at_end) {
        ++this->rest_iter;
      } else {
        ++this->sub_iter;
        if (!(this->sub_iter != this->sub_end)) {
          this->at_end = true;
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
      return this->sub_iter != other.sub_iter
             || this->rest_iter != other.rest_iter;
    }

    bool operator==(const Iterator& other) const {
      return !(*this != other);
    }

    iterator_deref<Container> operator*() {
      return this->at_end ? *this->rest_iter : *this->sub_iter;
    }

    iterator_arrow<Container> operator->() {
      return this->at_end ? apply_arrow(this->rest_iter)
                          : apply_arrow(this->sub_iter);
    }
  };

  Iterator begin() {
    return {std::begin(this->container), std::end(this->container),
        std::begin(this->rest_chained)};
  }

  Iterator end() {
    return {std::end(this->container), std::end(this->container),
        std::end(this->rest_chained)};
  }
};
template <typename Container>
class iter::impl::Chained<Container> {
  friend class ChainMaker;
  template <typename, typename...>
  friend class Chained;

 private:
  Container container;
  Chained(Container&& in_container)
      : container(std::forward<Container>(in_container)) {}

 public:
  Chained(Chained&&) = default;
  class Iterator : public std::iterator<std::input_iterator_tag,
                       iterator_traits_deref<Container>> {
   private:
    iterator_type<Container> sub_iter;
    iterator_type<Container> sub_end;

   public:
    Iterator(const iterator_type<Container>& s_begin,
        const iterator_type<Container>& s_end)
        : sub_iter{s_begin}, sub_end{s_end} {}

    Iterator& operator++() {
      ++this->sub_iter;
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    bool operator!=(const Iterator& other) const {
      return this->sub_iter != other.sub_iter;
    }

    bool operator==(const Iterator& other) const {
      return !(*this != other);
    }

    iterator_deref<Container> operator*() {
      return *this->sub_iter;
    }

    iterator_arrow<Container> operator->() {
      return apply_arrow(this->sub_iter);
    }
  };

  Iterator begin() {
    return {std::begin(this->container), std::end(this->container)};
  }

  Iterator end() {
    return {std::end(this->container), std::end(this->container)};
  }
};

template <typename Container>
class iter::impl::ChainedFromIterable {
 private:
  Container container;
  friend class ChainMaker;
  ChainedFromIterable(Container&& in_container)
      : container(std::forward<Container>(in_container)) {}

 public:
  ChainedFromIterable(ChainedFromIterable&&) = default;
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
      return std::unique_ptr<SubIter>{
          sub_iter ? new SubIter{*sub_iter} : nullptr};
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
                         : new SubIter{std::begin(*top_iter)}},
          sub_end_p{!(top_iter != top_end) ?  // iter == end ?
                        nullptr
                                           : new SubIter{std::end(*top_iter)}} {
    }

    Iterator(const Iterator& other)
        : top_level_iter{other.top_level_iter},
          top_level_end{other.top_level_end},
          sub_iter_p{clone_sub_pointer(other.sub_iter_p.get())},
          sub_end_p{clone_sub_pointer(other.sub_end_p.get())} {}

    Iterator& operator=(const Iterator& other) {
      if (this == &other) { return *this; }

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
          sub_iter_p.reset(new SubIter{std::begin(*this->top_level_iter)});
          sub_end_p.reset(new SubIter{std::end(*this->top_level_iter)});
        } else {
          sub_iter_p.reset(nullptr);
          sub_end_p.reset(nullptr);
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

    iterator_arrow<iterator_deref<Container>> operator->() {
      return apply_arrow(*this->sub_iter_p);
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
 public:
  // expose regular call operator to provide usual chain()
  template <typename... Containers>
  Chained<Containers...> operator()(Containers&&... cs) const {
    return {std::forward<Containers>(cs)...};
  }

  // chain.from_iterable()
  template <typename Container>
  ChainedFromIterable<Container> from_iterable(Container&& container) const {
    return {std::forward<Container>(container)};
  }
};

namespace iter {
  namespace {
    constexpr auto chain = impl::ChainMaker{};
  }
}

#endif
