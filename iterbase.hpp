#ifndef ITERBASE_HPP_
#define ITERBASE_HPP_


// This file consists of utilities used for the generic nature of the
// iterable wrapper classes.  As such, the contents of this file should be
// considered UNDOCUMENTED and is subject to change without warning.  This
// also applies to the name of the file.  No user code should include
// this file directly.

#include <utility>
#include <tuple>
#include <iterator>
#include <functional>
#include <cstddef>
#include <type_traits>

namespace iter {

    // iterator_type<C> is the type of C's iterator
    template <typename Container>
    using iterator_type =
        decltype(std::begin(std::declval<Container&>()));

    // iterator_deref<C> is the type obtained by dereferencing an iterator
    // to an object of type C
    template <typename Container>
    using iterator_deref = 
        decltype(*std::declval<iterator_type<Container>&>());

    template <typename Container>
    using iterator_traits_deref =
        typename std::remove_reference<iterator_deref<Container>>::type;

    // iterator_type<C> is the type of C's iterator
    template <typename Container>
    using reverse_iterator_type =
        decltype(std::declval<Container&>().rbegin());

    // iterator_deref<C> is the type obtained by dereferencing an iterator
    // to an object of type C
    template <typename Container>
    using reverse_iterator_deref = 
        decltype(*std::declval<reverse_iterator_type<Container>&>());

    template <typename, typename =void>
    struct is_random_access_iter : std::false_type { };

    template <typename T>
    struct is_random_access_iter<T,
        typename std::enable_if<
             std::is_same<
                 typename std::iterator_traits<T>::iterator_category,
              std::random_access_iterator_tag>::value,
          void>::type> : std::true_type { };

    template <typename T>
    using has_random_access_iter = is_random_access_iter<iterator_type<T>>;
    // because std::advance assumes a lot and is actually smart, I need a dumb

    // version that will work with most things
    template <typename InputIt, typename Distance =std::size_t>
    void dumb_advance(InputIt& iter, Distance distance=1) {
        for (Distance i(0); i < distance; ++i) {
            ++iter;
        }
    }

    template <typename Iter, typename Distance>
    void dumb_advance_impl(Iter& iter, const Iter& end,
            Distance distance, std::false_type) {
        for (Distance i(0); i < distance && iter != end; ++i) {
            ++iter;
        }
    }

    template <typename Iter, typename Distance>
    void dumb_advance_impl(Iter& iter, const Iter& end,
            Distance distance, std::true_type) {
        if (static_cast<Distance>(end - iter) < distance) {
            iter = end;
        } else {
            iter += distance;
        }
    }

    // iter will not be incremented past end
    template <typename Iter, typename Distance =std::size_t>
    void dumb_advance(Iter& iter, const Iter& end, Distance distance=1) {
        dumb_advance_impl(iter, end, distance, is_random_access_iter<Iter>{});
    }

    template <typename ForwardIt, typename Distance =std::size_t>
    ForwardIt dumb_next(ForwardIt it, Distance distance=1) {
        dumb_advance(it, distance);
        return it;
    }

    template <typename ForwardIt, typename Distance =std::size_t>
    ForwardIt dumb_next(
            ForwardIt it, const ForwardIt& end, Distance distance=1) {
        dumb_advance(it, end, distance);
        return it;
    }

    template <typename Container, typename Distance =std::size_t>
    Distance dumb_size(Container&& container) {
        Distance d{0};
        for (auto it = std::begin(container), end = std::end(container);
                it != end;
                ++it) {
            ++d;
        }
        return d;
    }


    template <typename... Ts>
    struct are_same : std::true_type { };

    template <typename T, typename U, typename... Ts>
    struct are_same<T, U, Ts...> 
        : std::integral_constant<bool,
            std::is_same<T, U>::value && are_same<T, Ts...>::value> { };

    namespace detail {
        template <typename... Ts>
        std::tuple<iterator_type<Ts>...> iterator_tuple_type_helper(
                const std::tuple<Ts...>&);
    }
    // Given a tuple template argument, evaluates to a tuple of iterators
    // for the template argument's contained types.
    template <typename TupleType>
    using iterator_tuple_type =
        decltype(detail::iterator_tuple_type_helper(
                    std::declval<TupleType>()));

    namespace detail {
        template <typename... Ts>
        std::tuple<iterator_deref<Ts>...> iterator_tuple_deref_helper(
                const std::tuple<Ts...>&);
    }

    // Given a tuple template argument, evaluates to a tuple of
    // what the iterators for the template argument's contained types
    // dereference to
    template <typename TupleType>
    using iterator_deref_tuple =
        decltype(detail::iterator_tuple_deref_helper(
                    std::declval<TupleType>()));

    // ---- Tuple utilities ---- //
    
    // function absorbing all arguments passed to it. used when
    // applying a function to a parameter pack but not passing the evaluated
    // results anywhere
    template <typename... Ts>
    void absorb(Ts&&...) { }

    namespace detail {
        template <typename Func, typename TupleType, std::size_t... Is>
        decltype(auto) call_with_tuple_impl(Func&& mf, TupleType&& tup,
                std::index_sequence<Is...>) {
            return mf(std::get<Is>(tup)...);
        }
    }

    // expand a TupleType into individual arguments when calling a Func
    template <typename Func, typename TupleType>
    decltype(auto) call_with_tuple(Func&& mf, TupleType&& tup) {
        constexpr auto TUP_SIZE = std::tuple_size<
            std::decay_t<TupleType>>::value;
        return detail::call_with_tuple_impl(
                std::forward<Func>(mf),
                std::forward<TupleType>(tup),
                std::make_index_sequence<TUP_SIZE>{});
    }
}

#endif
