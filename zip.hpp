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

    template <typename... RestContainers>
    class Zipped;

    template <typename... Containers>
    Zipped<Containers...> zip(Containers&&...);

    // specialization for at least 1 template argument
    template <typename... Containers>
    class Zipped {
        private:
            std::tuple<Containers...> containers;
            std::make_index_sequence<sizeof...(Containers)> indices;
            
        public:
            Zipped(Containers... in_containers)
                : containers{std::forward<Containers>(in_containers)...}
            { }

            class Iterator {
                private:
                    std::tuple<iterator_type<Containers>...> iters;
                    std::make_index_sequence<sizeof...(Containers)> indices;

                    bool not_equal(
                            const Iterator&, std::index_sequence<>) const {
                        return false;
                    }

                    template <std::size_t... Is>
                    bool not_equal(const Iterator& other,
                                   std::index_sequence<Is...>) const {
                        bool results[] = { true,
                            (std::get<Is>(this->iters) !=
                             std::get<Is>(other.iters))...
                        };
                        return std::all_of(
                                std::begin(results), std::end(results),
                                [](bool b){ return b; } );
                    }

                    template <std::size_t... Is>
                    void increment(std::index_sequence<Is...>) {
                        absorb(++std::get<Is>(this->iters)...);
                    }

                    template <std::size_t... Is>
                    auto deref(std::index_sequence<Is...>) {
                        return std::tuple<iterator_deref<Containers>...>{
                            (*std::get<Is>(this->iters))...};
                    }

                public:
                    Iterator(iterator_type<Containers>... its)
                        : iters{its...}
                    { }

                    Iterator& operator++() {
                        this->increment(this->indices);
                        return *this;
                    }

                    bool operator!=(const Iterator& other) const {
                        return this->not_equal(other, this->indices);
                    }

                    auto operator*() {
                        return this->deref(this->indices);
                    }
            };
        private:
            template <std::size_t... Is>
            Iterator make_begin(std::index_sequence<Is...>) {
                return {std::begin(std::get<Is>(this->containers))...};
            }

            template <std::size_t... Is>
            Iterator make_end(std::index_sequence<Is...>) {
                return {std::end(std::get<Is>(this->containers))...};
            }

        public:

            Iterator begin() {
                return this->make_begin(this->indices);
            }

            Iterator end() {
                return this->make_end(this->indices);
            }
        };

    template <typename... Containers>
    Zipped<Containers...> zip(Containers&&... containers) {
        return {std::forward<Containers>(containers)...};
    }
}

#endif // #ifndef ITER_ZIP_HPP_
