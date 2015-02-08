#ifndef ITER_STARMAP_H_
#define ITER_STARMAP_H_

#include "iterbase.hpp"

#include <utility>
#include <iterator>
#include <type_traits>
#include <array>
#include <cassert>
#include <memory>

namespace iter {

    // starmap with a container<T> where T is one of tuple, pair, array
    template <typename Func, typename Container>
    class StarMapper {
        private:
            Func func;
            Container container;

            using StarIterDeref =
                std::remove_reference_t<decltype(call_with_tuple(func,
                            std::declval<iterator_deref<Container>>()))>;

        public:
            StarMapper(Func f, Container&& c)
                : func(std::forward<Func>(f)),
                container(std::forward<Container>(c))
            { }

            class Iterator
                : public std::iterator<std::input_iterator_tag, StarIterDeref>
            {
                private:
                    Func func;
                    iterator_type<Container> sub_iter;
                public:
                    Iterator(Func& f, iterator_type<Container> iter)
                        : func(f),
                        sub_iter(iter)
                    { }

                    bool operator!=(const Iterator& other) const {
                        return this->sub_iter != other.sub_iter;
                    }

                    bool operator==(const Iterator& other) const {
                        return !(*this != other);
                    }

                    Iterator operator++() {
                        ++this->sub_iter;
                        return *this;
                    }

                    Iterator operator++(int) {
                        auto ret = *this;
                        ++*this;
                        return ret;
                    }

                    decltype(auto) operator*() {
                        return call_with_tuple(this->func, *this->sub_iter);
                    }
            };

            Iterator begin() {
                return {this->func, std::begin(this->container)};
            }

            Iterator end() {
                return {this->func, std::end(this->container)};
            }
    };



    template <typename Func, typename Container>
    StarMapper<Func, Container> starmap_helper(
            Func func, Container&& container, std::false_type) {
        return {std::forward<Func>(func), std::forward<Container>(container)};
    }

    // starmap for a tuple or pair of tuples or pairs
    template <typename Func, typename TupType, std::size_t... Is>
    class TupleStarMapper {
        private:
            Func func;
            TupType tup;

        private:
            static_assert(sizeof...(Is)
                    == std::tuple_size<std::decay_t<TupType>>::value,
                    "tuple size doesn't match size of Is");

            template <std::size_t Idx>
            static decltype(auto) get_and_call_with_tuple(Func& f, TupType &t){
                return call_with_tuple(f, std::get<Idx>(t));
            }

            using ResultType = decltype(get_and_call_with_tuple<0>(func, tup));
            using CallerFunc = ResultType (*)(Func&, TupType&);

            constexpr static std::array<CallerFunc, sizeof...(Is)> callers{{
                get_and_call_with_tuple<Is>...}};

            using TraitsValue = std::remove_reference_t<ResultType>;
        public:
            TupleStarMapper(Func f, TupType t)
                : func(std::forward<Func>(f)),
                tup(std::forward<TupType>(t))
            { }

            class Iterator 
                : public std::iterator<std::input_iterator_tag, TraitsValue> 
            {
                private:
                    Func& func;
                    TupType& tup;
                    std::size_t index;

                public:
                    Iterator(Func& f, TupType& t, std::size_t i)
                        : func{f},
                        tup{t},
                        index{i}
                    { }

                    decltype(auto) operator*() {
                        return callers[this->index](this->func, this->tup);
                    }

                    Iterator operator++() {
                        ++this->index;
                        return *this;
                    }

                    bool operator!=(const Iterator& other) const {
                        return this->index != other.index;
                    }
            };

            Iterator begin() {
                return {this->func, this->tup, 0};
            }

            Iterator end() {
                return {this->func, this->tup, sizeof...(Is)};
            }
    };
    template <typename Func, typename TupType, std::size_t... Is>
    constexpr std::array<
        typename TupleStarMapper<Func, TupType, Is...>::CallerFunc,
        sizeof...(Is)>
        TupleStarMapper<Func, TupType, Is...>::callers;



    template <typename Func, typename TupType, std::size_t... Is>
    TupleStarMapper<Func, TupType, Is...> starmap_helper_impl(
            Func func, TupType&& tup, std::index_sequence<Is...>)
    {
        return {std::forward<Func>(func), std::forward<TupType>(tup)};
    }

    template <typename Func, typename TupType>
    auto starmap_helper(
            Func func, TupType&& tup, std::true_type) {
        return starmap_helper_impl(
            std::forward<Func>(func),
            std::forward<TupType>(tup),
            std::make_index_sequence<
                std::tuple_size<std::decay_t<TupType>>::value>{});
    }


    // "tag dispatch" to differentiate between normal containers and
    // tuple-like containers, things that work with std::get
    template <typename T, typename =void>
    struct is_tuple_like : public std::false_type { };

    template <typename T>
    struct is_tuple_like<T, decltype(std::get<0>(std::declval<T>()), void())>
        : public std::true_type { };

    template <typename Func, typename Seq>
    auto starmap(Func func, Seq&& sequence) {
        return starmap_helper(
                std::forward<Func>(func),
                std::forward<Seq>(sequence),
                is_tuple_like<Seq>{});
    }
}

#endif
