#include "iterbase.hpp"

#include <iterator>
#include <tuple>
#include <utility>

namespace iter {
    template <typename Container, typename... RestContainers>
    class Zipped {
        static_assert(!std::is_rvalue_reference<Container>::value,
                "Itertools cannot be templated with rvalue references");
        private:
            Container container;
            Zipped<RestContainers...> rest_zipped;
        public:
            Zipped(Container container, RestContainers&&... rest)
                : container(std::forward<Container>(container)),
                rest_zipped{std::forward<RestContainers>(rest)...}
            { }

            class Iterator {
                private:
                    using RestIter =
                        typename Zipped<RestContainers...>::Iterator;

                    iterator_type<Container> iter;
                    RestIter rest_iter;
                public:
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
                            this->rest_iter != other.rest_iter;
                    }

                    auto operator*() ->
                        decltype(std::tuple_cat(std::make_tuple(
                                        *this->iter), *this->rest_iter))
                    {
                        return std::tuple_cat(std::make_tuple(
                                    *this->iter), *this->rest_iter);
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


    template <typename Container>
    class Zipped<Container> {
        static_assert(!std::is_rvalue_reference<Container>::value,
                "Itertools cannot be templated with rvalue references");
        private:
            Container container;
        public:
            Zipped(Container container)
                : container(std::forward<Container>(container))
            { }

            class Iterator {
                private:
                    iterator_type<Container> iter;
                public:
                    Iterator(iterator_type<Container> it)
                        : iter{it}
                    { }

                    Iterator& operator++() {
                        ++this->iter;
                        return *this;
                    }

                    bool operator!=(const Iterator& other) const {
                        // if this->iter == other.iter, then won't every other
                        // level have to be the same? 
                        // in other words, I should only have to compare iter,
                        // and not rest as well
                        return this->iter != other.iter;
                    }

                    auto operator*() -> decltype(std::make_tuple(*this->iter))
                    {
                        return std::make_tuple(*this->iter);
                    }
            };

            Iterator begin() {
                return {std::begin(this->container)};
            }

            Iterator end() {
                return {std::end(this->container)};
            }
    };

    template <typename... Containers>
    Zipped<Containers...> zip(Containers&&... containers) {
        return {std::forward<Containers>(containers)...};
    }
}
