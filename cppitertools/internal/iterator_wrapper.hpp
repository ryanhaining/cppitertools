#ifndef ITERTOOLS_ITERATOR_WRAPPER_HPP_
#define ITERTOOLS_ITERATOR_WRAPPER_HPP_

#include <cassert>
#include <functional>
#include <variant>
#include "iterbase.hpp"

namespace iter {
  namespace impl {
    // iterator_end_type<C> is the type of C's end iterator
    template <typename Container>
    using iterator_end_type = decltype(get_end(std::declval<Container&>()));

    template <typename SubIter, typename SubEnd>
    class IteratorWrapperImpl;

    // If begin and end return the same type, type will be
    // iterator_type<Container>
    // If begin and end return different types, type will be IteratorWrapperImpl
    template <typename Container, bool same_types>
    struct IteratorWrapperImplType;

    template <typename Container>
    struct IteratorWrapperImplType<Container, true>
        : type_is<iterator_type<Container>> {};

    template <typename Container>
    struct IteratorWrapperImplType<Container, false>
        : type_is<IteratorWrapperImpl<iterator_type<Container>,
              iterator_end_type<Container>>> {};

    template <typename Container>
    using IteratorWrapper = typename IteratorWrapperImplType<Container,
        std::is_same_v<impl::iterator_type<Container>,
            impl::iterator_end_type<Container>>>::type;
  }
}

template <typename SubIter, typename SubEnd>
class iter::impl::IteratorWrapperImpl {
 private:
  static_assert(!std::is_same_v<SubIter, SubEnd>);
  SubIter& sub_iter() {
    auto* sub = std::get_if<SubIter>(&sub_iter_or_end_);
    assert(sub);
    return *sub;
  }

  const SubIter& sub_iter() const {
    auto* sub = std::get_if<SubIter>(&sub_iter_or_end_);
    assert(sub);
    return *sub;
  }

  std::variant<SubIter, SubEnd> sub_iter_or_end_;

 public:
  IteratorWrapperImpl() : IteratorWrapperImpl(SubIter{}) {}

  IteratorWrapperImpl(SubIter&& it) : sub_iter_or_end_{std::move(it)} {}

  IteratorWrapperImpl(SubEnd&& it) : sub_iter_or_end_(std::move(it)) {}

  IteratorWrapperImpl& operator++() {
    ++sub_iter();
    return *this;
  }

  decltype(auto) operator*() {
    return *sub_iter();
  }

  decltype(auto) operator*() const {
    return *sub_iter();
  }

  decltype(auto) operator-> () {
    return apply_arrow(sub_iter());
  }

  decltype(auto) operator-> () const {
    return apply_arrow(sub_iter());
  }

  bool operator!=(const IteratorWrapperImpl& other) const {
    constexpr static struct : std::not_equal_to<void> {
      // specially compare Ends because rangev3 sentinels are not equality
      // comparable
      bool operator()(const SubEnd&, const SubEnd&) const {
        return false;
      }
      using std::not_equal_to<void>::operator();
    } not_equal;
    return std::visit(not_equal, sub_iter_or_end_, other.sub_iter_or_end_);
  }

  bool operator==(const IteratorWrapperImpl& other) const {
    return !(*this != other);
  }
};

#endif
