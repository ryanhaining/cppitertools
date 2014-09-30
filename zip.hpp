#ifndef ITER_ZIP_HPP_
#define ITER_ZIP_HPP_

#include "iterbase.hpp"

#include <iterator>
#include <tuple>
#include <utility>
#include <algorithm>

namespace iter {

    template <typename... Ts>
    void absorb(Ts&&...) { }

    template <typename TupleType, std::size_t... Is>
    class Zipped;

    template <typename TupleType, std::size_t... Is>
    Zipped<TupleType, Is...> zip_impl(TupleType&&, std::index_sequence<Is...>);

    template <typename TupleType, std::size_t... Is>
    class Zipped {
        private:
            TupleType containers;
            using iters_tuple =
                std::tuple<iterator_type<decltype(
                        std::get<Is>(std::declval<TupleType>()))>...>;
            friend Zipped zip_impl<TupleType, Is...>(
                    TupleType&&, std::index_sequence<Is...>);

            Zipped(TupleType&& in_containers)
                : containers(std::move(in_containers))
            { }
        public:

            class Iterator {
                private:
                    using iters_tuple =
                        std::tuple<iterator_type<decltype(
                                std::get<Is>(std::declval<TupleType>()))>...>;
                    using iters_deref_tuple =
                        std::tuple<iterator_deref<decltype(
                                std::get<Is>(std::declval<TupleType>()))>...>;
                    iters_tuple iters;

                public:
                    Iterator(iters_tuple&& its)
                        : iters(std::move(its))
                    { }

                    Iterator& operator++() {
                        absorb(++std::get<Is>(this->iters)...);
                        return *this;
                    }

                    bool operator!=(const Iterator& other) const {
                        if (sizeof...(Is) == 0) return false;

                        bool results[] = { true,
                            (std::get<Is>(this->iters) !=
                                 std::get<Is>(other.iters))...
                        };
                        return std::all_of(
                                std::begin(results), std::end(results),
                                [](bool b){ return b; } );
                    }

                    auto operator*() {
                        return iters_deref_tuple{
                            (*std::get<Is>(this->iters))...};
                    }
            };

            Iterator begin() {
                return iters_tuple{
                    std::begin(std::get<Is>(this->containers))...};
            }

            Iterator end() {
                return iters_tuple{
                    std::end(std::get<Is>(this->containers))...};
            }
    };

    template <typename TupleType, std::size_t... Is>
    Zipped<TupleType, Is...> zip_impl(
            TupleType&& in_containers, std::index_sequence<Is...>) {
        return {std::move(in_containers)};
    }

    template <typename... Containers>
    auto zip(Containers&&... containers) {
        return zip_impl(std::tuple<Containers...>{
            std::forward<Containers>(containers)...},
            std::index_sequence_for<Containers...>{});
    }
}

#endif // #ifndef ITER_ZIP_HPP_
