#ifndef ITER_PRODUCT_HPP_
#define ITER_PRODUCT_HPP_

#include "iterbase.hpp"

#include <iterator>
#include <tuple>
#include <utility>


namespace iter {
    template <typename... RestContainers>
    class Productor;

    template <typename... Containers>
    Productor<Containers...> product(Containers&&...);

    // specialization for at least 1 template argument
    template <typename Container, typename... RestContainers>
    class Productor <Container, RestContainers...> {
        static_assert(!std::is_rvalue_reference<Container>::value,
                "Itertools cannot be templated with rvalue references");

        friend Productor product<Container, RestContainers...>(
                Container&&, RestContainers&&...);

        template <typename... RC>
        friend class Productor;

        private:
            Container container;
            Productor<RestContainers...> rest_products;
            Productor(Container container, RestContainers&&... rest)
                : container(std::forward<Container>(container)),
                rest_products{std::forward<RestContainers>(rest)...}
            { }

        public:
            class Iterator {
                private:
                    using RestIter =
                        typename Productor<RestContainers...>::Iterator;

                    iterator_type<Container> iter;
                    const iterator_type<Container> begin;

                    RestIter rest_iter;
                    const RestIter rest_end;
                public:
                    constexpr static const bool is_base_iter = false;
                    Iterator(iterator_type<Container> it,
                            const RestIter& rest,
                            const RestIter& in_rest_end)
                        : iter{it},
                        begin{it},
                        rest_iter{rest},
                        rest_end{in_rest_end}
                    { }

                    void reset() {
                        this->iter = this->begin;
                    }

                    Iterator& operator++() {
                        ++this->rest_iter;
                        if (!(this->rest_iter != this->rest_end)) {
                            this->rest_iter.reset();
                            ++this->iter;
                        }
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
                    std::begin(this->rest_products),
                    std::end(this->rest_products)};
            }

            Iterator end() {
                return {std::end(this->container),
                    std::end(this->rest_products),
                    std::end(this->rest_products)};
            }
    };


    template <>
    class Productor<> {
        public:
            class Iterator {
                public:
                    constexpr static const bool is_base_iter = true;

                    Iterator() { }
                    Iterator(const Iterator&) { }
                    Iterator& operator=(const Iterator&) { return *this; }

                    void reset() { }

                    Iterator& operator++() {
                        return *this;
                    }

                    // see note in zip about base case operator!=
                    bool operator!=(const Iterator&) const {
                        return false;
                    }

                    std::tuple<> operator*() const {
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
    Productor<Containers...> product(Containers&&... containers) {
        return {std::forward<Containers>(containers)...};
    }
}

#endif // #ifndef ITER_PRODUCT_HPP_
