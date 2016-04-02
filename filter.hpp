#ifndef ITER_FILTER_H_
#define ITER_FILTER_H_

#include "internal/iterbase.hpp"

#include <utility>
#include <iterator>
#include <initializer_list>

namespace iter {
  namespace impl {
    template <typename FilterFunc, typename Container>
    class Filtered;

    class FilterFn;
  }
}

template <typename FilterFunc, typename Container>
class iter::impl::Filtered {
 private:
  Container container;
  FilterFunc filter_func;

  friend class FilterFn;

  // Value constructor for use only in the filter function
  Filtered(FilterFunc in_filter_func, Container&& in_container)
      : container(std::forward<Container>(in_container)),
        filter_func(in_filter_func) {}

 public:
  Filtered(Filtered&&) = default;

  class Iterator : public std::iterator<std::input_iterator_tag,
                       iterator_traits_deref<Container>> {
   protected:
    using Holder = DerefHolder<iterator_deref<Container>>;
    iterator_type<Container> sub_iter;
    iterator_type<Container> sub_end;
    Holder item;
    FilterFunc* filter_func;

    void inc_sub_iter() {
      ++this->sub_iter;
      if (this->sub_iter != this->sub_end) {
        this->item.reset(*this->sub_iter);
      }
    }

    // increment until the iterator points to is true on the
    // predicate.  Called by constructor and operator++
    void skip_failures() {
      while (this->sub_iter != this->sub_end
             && !(*this->filter_func)(this->item.get())) {
        this->inc_sub_iter();
      }
    }

   public:
    Iterator(iterator_type<Container> iter, iterator_type<Container> end,
        FilterFunc& in_filter_func)
        : sub_iter{iter}, sub_end{end}, filter_func(&in_filter_func) {
      if (this->sub_iter != this->sub_end) {
        this->item.reset(*this->sub_iter);
      }
      this->skip_failures();
    }

    typename Holder::reference operator*() {
      return this->item.get();
    }

    typename Holder::pointer operator->() {
      return this->item.get_ptr();
    }

    Iterator& operator++() {
      this->inc_sub_iter();
      this->skip_failures();
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
  };

  Iterator begin() {
    return {std::begin(this->container), std::end(this->container),
        this->filter_func};
  }

  Iterator end() {
    return {std::end(this->container), std::end(this->container),
        this->filter_func};
  }
};

class iter::impl::FilterFn : public iter::impl::Pipeable<FilterFn> {
 public:
  template <typename FilterFunc, typename Container>
  iter::impl::Filtered<FilterFunc, Container> operator()(
      FilterFunc filter_func, Container&& container) const {
    return {std::move(filter_func), std::forward<Container>(container)};
  }

  template <typename FilterFunc, typename T>
  iter::impl::Filtered<FilterFunc, std::initializer_list<T>> operator()(
      FilterFunc filter_func, std::initializer_list<T> il) const {
    return {std::move(filter_func), std::move(il)};
  }

  template <typename Container,
      typename = std::enable_if_t<is_iterable<Container>>>
  auto operator()(Container&& container) const {
    return (*this)(BoolTester<Container>{}, std::forward<Container>(container));
  }

  template <typename T>
  auto operator()(std::initializer_list<T> il) const {
    return (*this)(BoolTester<std::initializer_list<T>>{}, std::move(il));
  }

  template <typename FilterFunc>
  struct FilterFnPartial : Pipeable<FilterFnPartial<FilterFunc>> {
    FilterFnPartial(FilterFunc f) : fun(std::move(f)) {}
    template <typename Container>
    auto operator()(Container&& container) const {
      return FilterFn{}(fun, std::forward<Container>(container));
    }
   private:
    FilterFunc fun;
  };

  template <typename FilterFunc, typename=std::enable_if_t<!is_iterable<FilterFunc>>>
  FilterFnPartial<FilterFunc> operator()(FilterFunc filter_func) const {
    return {filter_func};
  }

 private:
  template <typename T>
  bool boolean_cast(const T& t) {
    return bool(t);
  }

  template <typename Container>
  class BoolTester {
   public:
    bool operator()(const impl::iterator_deref<Container> item) const {
      return bool(item);
    }
  };
};

namespace iter {
  constexpr impl::FilterFn filter{};
}

#endif
