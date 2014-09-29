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

#if 0
    template <typename... RestContainers>
    class Zipped;

    template <typename... Containers>
    Zipped<Containers...> zip(Containers&&...);
#endif

    // specialization for at least 1 template argument
    template <typename TupType, std::size_t... Is>
    class Zipped {
        private:
            TupType containers;
            using iters_tuple =
                std::tuple<iterator_type<decltype(
                        std::get<Is>(std::declval<TupType>()))>...>;
        public:
            Zipped(TupType&& in_containers)
                : containers(std::move(in_containers))
            { }

            class Iterator {
                private:
                    using iters_tuple =
                        std::tuple<iterator_type<decltype(
                                std::get<Is>(std::declval<TupType>()))>...>;
                    using iters_deref_tuple =
                        std::tuple<iterator_deref<decltype(
                                std::get<Is>(std::declval<TupType>()))>...>;

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

    template <typename TupType, std::size_t... Is>
    Zipped<TupType, Is...> zip_impl(
            TupType&& in_containers, std::index_sequence<Is...>) {
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
