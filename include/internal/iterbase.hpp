#ifndef ITERBASE_HPP_
#define ITERBASE_HPP_

// This file consists of utilities used for the generic nature of the
// iterable wrapper classes.  As such, the contents of this file should be
// considered UNDOCUMENTED and is subject to change without warning.  This
// also applies to the name of the file.  No user code should include
// this file directly.

#include <cassert>
#include <cstddef>
#include <functional>
#include <iterator>
#include <optional>
#include <tuple>
#include <type_traits>
#include <utility>

// see gcc bug 87651
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=87651
#ifdef __GNUC__
#define NO_GCC_FRIEND_ERROR __GNUC__ < 8
#else
#define NO_GCC_FRIEND_ERROR 1
#endif

namespace iter {
  namespace impl {
    namespace get_iters {
      // begin() for C arrays
      template <typename T, std::size_t N>
      T* get_begin_impl(T (&array)[N], int) {
        return array;
      }

      // Prefer member begin().
      template <typename T, typename I = decltype(std::declval<T&>().begin())>
      I get_begin_impl(T& r, int) {
        return r.begin();
      }

      // Use ADL otherwises.
      template <typename T, typename I = decltype(begin(std::declval<T&>()))>
      I get_begin_impl(T& r, long) {
        return begin(r);
      }

      template <typename T>
      auto get_begin(T& t) -> decltype(get_begin_impl(std::declval<T&>(), 42)) {
        return get_begin_impl(t, 42);
      }

      // end() for C arrays
      template <typename T, std::size_t N>
      T* get_end_impl(T (&array)[N], int) {
        return array + N;
      }

      // Prefer member end().
      template <typename T, typename I = decltype(std::declval<T&>().end())>
      I get_end_impl(T& r, int) {
        return r.end();
      }

      // Use ADL otherwise.
      template <typename T, typename I = decltype(end(std::declval<T&>()))>
      I get_end_impl(T& r, long) {
        return end(r);
      }

      template <typename T>
      auto get_end(T& t) -> decltype(get_end_impl(std::declval<T&>(), 42)) {
        return get_end_impl(t, 42);
      }
    }
    using get_iters::get_begin;
    using get_iters::get_end;

    template <typename T>
    struct type_is {
      using type = T;
    };

    template <typename T>
    using AsConst = decltype(std::as_const(std::declval<T&>()));

    // iterator_type<C> is the type of C's iterator
    // TODO: See bug
    // https://developercommunity.visualstudio.com/content/problem/252157/sfinae-error-depends-on-name-of-template-parameter.html
    // for why we use T instead of Container.  Should be
    // changed back to Container when that bug is fixed in
    // MSVC.
    template <typename T>
    using iterator_type = decltype(get_begin(std::declval<T&>()));

    // iterator_type<C> is the type of C's iterator
    template <typename Container>
    using const_iterator_type = decltype(
        get_begin(std::declval<const std::remove_reference_t<Container>&>()));

    // iterator_deref<C> is the type obtained by dereferencing an iterator
    // to an object of type C
    template <typename Container>
    using iterator_deref = decltype(*std::declval<iterator_type<Container>&>());

    // const_iteator_deref is the type obtained through dereferencing
    // a const iterator& (note: not a const_iterator).  ie: the result
    // of Container::iterator::operator*() const
    template <typename Container>
    using const_iterator_deref =
        decltype(*std::declval<const iterator_type<Container>&>());

    // the type of dereferencing a const_iterator
    template <typename Container>
    using const_iterator_type_deref =
        decltype(*std::declval<const_iterator_type<Container>&>());

    template <typename Container>
    using iterator_traits_deref =
        std::remove_reference_t<iterator_deref<Container>>;

    template <typename T, typename = void>
    struct IsIterable : std::false_type {};

    // Assuming that if a type works with begin, it is an iterable.
    template <typename T>
    struct IsIterable<T, std::void_t<iterator_type<T>>> : std::true_type {};

    template <typename T>
    constexpr bool is_iterable = IsIterable<T>::value;

    namespace detail {
      template <typename T, typename = void>
      struct ArrowHelper {
        using type = void;
        void operator()(T&) const noexcept {}
      };

      template <typename T>
      struct ArrowHelper<T*, void> {
        using type = T*;
        constexpr type operator()(T* t) const noexcept {
          return t;
        }
      };

      template <typename T>
      struct ArrowHelper<T,
          std::void_t<decltype(std::declval<T&>().operator->())>> {
        using type = decltype(std::declval<T&>().operator->());
        type operator()(T& t) const {
          return t.operator->();
        }
      };

      template <typename T>
      using arrow = typename detail::ArrowHelper<T>::type;
    }

    // type of C::iterator::operator->, also works with pointers
    // void if the iterator has no operator->
    template <typename C>
    using iterator_arrow = detail::arrow<iterator_type<C>>;

    // applys the -> operator to an object, if the object is a pointer,
    // it returns the pointer
    template <typename T>
    detail::arrow<T> apply_arrow(T& t) {
      return detail::ArrowHelper<T>{}(t);
    }

    // For iterators that have an operator* which returns a value
    // they can return this type from their operator-> instead, which will
    // wrap an object and allow it to be used with arrow
    template <typename T>
    class ArrowProxy {
     private:
      using TPlain = typename std::remove_reference<T>::type;
      T obj;

     public:
      constexpr ArrowProxy(T&& in_obj) : obj(std::forward<T>(in_obj)) {}

      TPlain* operator->() {
        return &obj;
      }
    };

    template <typename, typename = void>
    struct is_random_access_iter : std::false_type {};

    template <typename T>
    struct is_random_access_iter<T,
        std::enable_if_t<
            std::is_same<typename std::iterator_traits<T>::iterator_category,
                std::random_access_iterator_tag>::value>> : std::true_type {};

    template <typename T>
    using has_random_access_iter = is_random_access_iter<iterator_type<T>>;
    // because std::advance assumes a lot and is actually smart, I need a dumb

    // version that will work with most things
    template <typename InputIt, typename Distance = std::size_t>
    void dumb_advance_unsafe(InputIt& iter, Distance distance) {
      for (Distance i(0); i < distance; ++i) {
        ++iter;
      }
    }

    template <typename Iter, typename EndIter, typename Distance>
    void dumb_advance_impl(
        Iter& iter, const EndIter& end, Distance distance, std::false_type) {
      for (Distance i(0); i < distance && iter != end; ++i) {
        ++iter;
      }
    }

    template <typename Iter, typename EndIter, typename Distance>
    void dumb_advance_impl(
        Iter& iter, const EndIter& end, Distance distance, std::true_type) {
      if (static_cast<Distance>(end - iter) < distance) {
        iter = end;
      } else {
        iter += distance;
      }
    }

    // iter will not be incremented past end
    template <typename Iter, typename EndIter, typename Distance = std::size_t>
    void dumb_advance(Iter& iter, const EndIter& end, Distance distance) {
      dumb_advance_impl(iter, end, distance, is_random_access_iter<Iter>{});
    }

    template <typename ForwardIt, typename Distance = std::size_t>
    ForwardIt dumb_next(ForwardIt it, Distance distance = 1) {
      dumb_advance_unsafe(it, distance);
      return it;
    }

    template <typename ForwardIt, typename Distance = std::size_t>
    ForwardIt dumb_next(
        ForwardIt it, const ForwardIt& end, Distance distance = 1) {
      dumb_advance(it, end, distance);
      return it;
    }

    template <typename Container, typename Distance = std::size_t>
    Distance dumb_size(Container&& container) {
      Distance d{0};
      auto end_it = get_end(container);
      for (auto it = get_begin(container); it != end_it; ++it) {
        ++d;
      }
      return d;
    }

    template <typename... Ts>
    struct are_same : std::true_type {};

    template <typename T, typename U, typename... Ts>
    struct are_same<T, U, Ts...>
        : std::integral_constant<bool,
              std::is_same<T, U>::value && are_same<T, Ts...>::value> {};

    // DerefHolder holds the value gotten from an iterator dereference
    // if the iterate dereferences to an lvalue references, a pointer to the
    //     element is stored
    // if it does not, a value is stored instead
    // get() returns a reference to the held item
    // get_ptr() returns a pointer to the held item
    // reset() replaces the currently held item
    template <typename T>
    class DerefHolder {
     private:
      static_assert(!std::is_lvalue_reference<T>::value,
          "Non-lvalue-ref specialization used for lvalue ref type");
      // it could still be an rvalue reference
      using TPlain = std::remove_reference_t<T>;

      std::optional<TPlain> item_p_;

     public:
      using reference = TPlain&;
      using pointer = TPlain*;

      static constexpr bool stores_value = true;

      DerefHolder() = default;

      reference get() {
        assert(item_p_.has_value());
        return *item_p_;
      }

      pointer get_ptr() {
        assert(item_p_.has_value());
        return &item_p_.value();
      }

      void reset(T&& item) {
        item_p_.emplace(std::move(item));
      }

      explicit operator bool() const {
        return static_cast<bool>(item_p_);
      }
    };

    // Specialization for when T is an lvalue ref
    template <typename T>
    class DerefHolder<T&> {
     public:
      using reference = T&;
      using pointer = T*;

     private:
      pointer item_p_{};

     public:
      static constexpr bool stores_value = false;

      DerefHolder() = default;

      reference get() {
        assert(item_p_);
        return *item_p_;
      }

      pointer get_ptr() {
        assert(item_p_);
        return item_p_;
      }

      void reset(reference item) {
        item_p_ = &item;
      }

      explicit operator bool() const {
        return item_p_ != nullptr;
      }
    };

    // allows f(x) to be 'called' as x | f
    // let the record show I dislike adding yet another syntactical mess to
    // this clown car of a language.
    template <typename ItTool>
    struct Pipeable {
      template <typename T>
      friend decltype(auto) operator|(T&& x, const Pipeable& p) {
        return static_cast<const ItTool&>(p)(std::forward<T>(x));
      }
    };

    // Pipeable Callable generator, where ItImpl is templated on the first
    // argument to the call.
    template <template <typename> class ItImpl>
    struct IterToolFn : Pipeable<IterToolFn<ItImpl>> {
      template <typename T, typename... Ts>
      ItImpl<T> operator()(T&& t, Ts... ts) const {
        return {std::forward<T>(t), std::move(ts)...};
      }
    };

    // Pipeable callable which allows binding of the first argument
    // f(a, b) is the same as b | f(a)
    template <typename F>
    struct PipeableAndBindFirst : Pipeable<F> {
     protected:
      template <typename T>
      struct FnPartial : Pipeable<FnPartial<T>> {
        mutable T stored_arg;
        constexpr FnPartial(T in_t) : stored_arg(in_t) {}

        template <typename Container>
        auto operator()(Container&& container) const {
          return F{}(stored_arg, std::forward<Container>(container));
        }
      };

     public:
      template <typename T, typename = std::enable_if_t<!is_iterable<T>>>
      FnPartial<std::decay_t<T>> operator()(T&& t) const {
        return {std::forward<T>(t)};
      }
    };

    // This is a complicated class to generate a callable that can work:
    //  (1) with just a single (iterable) passed, and DefaultT substituted
    //  (2) with an iterable and a callable
    //  (3) with just a callable, to have the iterable passed later via pipe
    template <template <typename, typename> class ItImpl, typename DefaultT>
    struct IterToolFnOptionalBindFirst
        : PipeableAndBindFirst<IterToolFnOptionalBindFirst<ItImpl, DefaultT>> {
     private:
      using Base =
          PipeableAndBindFirst<IterToolFnOptionalBindFirst<ItImpl, DefaultT>>;

     protected:
      template <typename Container>
      auto operator()(Container&& container, std::false_type) const {
        return static_cast<const Base&>(*this)(
            std::forward<Container>(container));
      }

      template <typename Container>
      auto operator()(Container&& container, std::true_type) const {
        return (*this)(DefaultT{}, std::forward<Container>(container));
      }

     public:
      template <typename T>
      auto operator()(T&& t) const {
        return (*this)(std::forward<T>(t), IsIterable<T>{});
      }

      template <typename T, typename Container,
          typename = std::enable_if_t<is_iterable<Container>>>
      ItImpl<T, Container> operator()(T func, Container&& container) const {
        return {std::move(func), std::forward<Container>(container)};
      }
    };

    template <template <typename, typename> class ItImpl, typename DefaultT>
    struct IterToolFnOptionalBindSecond
        : Pipeable<IterToolFnOptionalBindSecond<ItImpl, DefaultT>> {
     private:
      // T is whatever is being held for later use
      template <typename T>
      struct FnPartial : Pipeable<FnPartial<T>> {
        mutable T stored_arg;
        constexpr FnPartial(T in_t) : stored_arg(in_t) {}

        template <typename Container>
        auto operator()(Container&& container) const {
          return IterToolFnOptionalBindSecond{}(
              std::forward<Container>(container), stored_arg);
        }
      };

     public:
      template <typename Container, typename T>
      ItImpl<Container, T> operator()(Container&& container, T func) const {
        return {std::forward<Container>(container), std::move(func)};
      }

      template <typename T, typename = std::enable_if_t<!is_iterable<T>>>
      FnPartial<std::decay_t<T>> operator()(T&& func) const {
        return {std::forward<T>(func)};
      }

      template <typename Container,
          typename = std::enable_if_t<is_iterable<Container>>>
      auto operator()(Container&& container) const {
        return (*this)(std::forward<Container>(container), DefaultT{});
      }
    };

    template <template <typename> class ItImpl>
    struct IterToolFnBindSizeTSecond {  // NOTE not pipeable
     private:
      using Size = std::size_t;
      struct FnPartial : Pipeable<FnPartial> {
        Size sz{};
        constexpr FnPartial(Size in_sz) : sz{in_sz} {}

        template <typename Container>
        auto operator()(Container&& container) const {
          return IterToolFnBindSizeTSecond{}(
              std::forward<Container>(container), sz);
        }
      };

     public:
      FnPartial operator()(Size sz) const {
        return {sz};
      }

      template <typename Container,
          typename = std::enable_if_t<is_iterable<Container>>>
      ItImpl<Container> operator()(Container&& container, Size sz) const {
        return {std::forward<Container>(container), sz};
      }
    };
  }
}

#endif
