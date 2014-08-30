#ifndef ITER_ZIP_HPP_
#define ITER_ZIP_HPP_

#include "iterbase.hpp"

#include <iterator>
#include <tuple>
#include <utility>


namespace iter {
    template <typename... RestContainers>
    class Zipped;

    template <typename... Containers>
    Zipped<Containers...> zip(Containers&&...);

    // specialization for at least 1 template argument
    template <typename Container, typename... RestContainers>
    class Zipped <Container, RestContainers...> {
        static_assert(!std::is_rvalue_reference<Container>::value,
                "Itertools cannot be templated with rvalue references");

        friend Zipped zip<Container, RestContainers...>(
                Container&&, RestContainers&&...);

        template <typename... RC>
        friend class Zipped;

        private:
            Container container;
            Zipped<RestContainers...> rest_zipped;
            Zipped(Container container, RestContainers&&... rest)
                : container(std::forward<Container>(container)),
                rest_zipped{std::forward<RestContainers>(rest)...}
            { }

        public:
            class Iterator {
                private:
                    using RestIter =
                        typename Zipped<RestContainers...>::Iterator;

                    iterator_type<Container> iter;
                    RestIter rest_iter;
                public:
                    constexpr static const bool is_base_iter = false;
                    Iterator(iterator_type<Container> it, const RestIter& rest)
                        : iter{it},
                        rest_iter{rest}
                    { }

                    Iterator& operator++() {
                        ++this->iter;
                        ++this->rest_iter;
                        return *this;
                    }

                    bool operator!=(const Iterator& other) const {
                        return this->iter != other.iter &&
                            (RestIter::is_base_iter ||
                                this->rest_iter != other.rest_iter);
                    }

                    auto operator*() ->
                        decltype(std::tuple_cat(
                                    std::tuple<iterator_deref<Container>>{
                                        *this->iter},
                                    *this->rest_iter))
                    {
                        return std::tuple_cat(
                                std::tuple<iterator_deref<Container>>{
                                    *this->iter},
                                *this->rest_iter);
                    }
            };

            Iterator begin() {
                return {std::begin(this->container),
                    std::begin(this->rest_zipped)};
            }

            Iterator end() {
                return {std::end(this->container),
                    std::end(this->rest_zipped)};
            }
    };


    template <>
    class Zipped<> {
        public:
            class Iterator {
                public:
                    constexpr static const bool is_base_iter = true;

                    Iterator() { }
                    Iterator(const Iterator&) { }
                    Iterator& operator=(const Iterator&) { return *this; }

                    Iterator& operator++() {
                        return *this;
                    }

                    // if this were to return true, there would be no need
                    // for the is_base_iter static class attribute.
                    // However, returning false causes an empty zip() call
                    // to reach the "end" immediately. Returning true here
                    // instead results in an infinite loop in the zip() case
                    bool operator!=(const Iterator&) const {
                        return false;
                    }

                    std::tuple<> operator*() {
                        return std::tuple<>{};
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
    Zipped<Containers...> zip(Containers&&... containers) {
        return {std::forward<Containers>(containers)...};
    }
}

#endif // #ifndef ITER_ZIP_HPP_
