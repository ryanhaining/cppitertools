#ifndef ZIP_LONGEST_HPP_
#define ZIP_LONGEST_HPP_

#include "iterbase.hpp"

#include <boost/optional.hpp>
#include <iterator>
#include <tuple>
#include <utility>


namespace iter {
    template <typename Container, typename... RestContainers>
    class ZippedLongest;

    template <typename... Containers>
    ZippedLongest<Containers...> zip_longest(Containers&&...);

    template <typename Container, typename... RestContainers>
    class ZippedLongest {
        static_assert(!std::is_rvalue_reference<Container>::value,
                "Itertools cannot be templated with rvalue references");

        friend ZippedLongest zip_longest<Container, RestContainers...>(
                Container&&, RestContainers&&...);

        template <typename C, typename... RC>
        friend class ZippedLongest;

        private:
            Container container;
            ZippedLongest<RestContainers...> rest_zipped;
            ZippedLongest(Container container, RestContainers&&... rest)
                : container(std::forward<Container>(container)),
                rest_zipped{std::forward<RestContainers>(rest)...}
            { }

        public:
            class Iterator {
                private:
                    using RestIter =
                        typename ZippedLongest<RestContainers...>::Iterator;
                    using OptType = boost::optional<iterator_deref<Container>>;

                    iterator_type<Container> iter;
                    iterator_type<Container> end;
                    RestIter rest_iter;

                public:
                    Iterator(
                            iterator_type<Container> it,
                            iterator_type<Container> in_end,
                            const RestIter& rest)
                        : iter{it},
                        end{in_end},
                        rest_iter{rest}
                    { }

                    Iterator& operator++() {
                        if (this->iter != this->end) {
                            ++this->iter;
                        }
                        ++this->rest_iter;
                        return *this;
                    }

                    bool operator!=(const Iterator& other) const {
                        return this->iter != other.iter ||
                            this->rest_iter != other.rest_iter;
                    }

                    auto operator*() ->
                        decltype(std::tuple_cat(
                                    std::tuple<OptType>{OptType{*this->iter}},
                                    *this->rest_iter))
                    {
                        if (this->iter != this->end) {
                            return std::tuple_cat(
                                    std::tuple<OptType>{OptType{*this->iter}},
                                    *this->rest_iter);
                        } else {
                            return std::tuple_cat(
                                    std::tuple<OptType>{OptType{}},
                                    *this->rest_iter);
                        }
                    }
            };

            Iterator begin() {
                return {std::begin(this->container),
                    std::end(this->container),
                    std::begin(this->rest_zipped)};
            }

            Iterator end() {
                return {std::end(this->container),
                    std::end(this->container),
                    std::end(this->rest_zipped)};
            }
    };


    template <typename Container>
    class ZippedLongest<Container> {
        static_assert(!std::is_rvalue_reference<Container>::value,
                "Itertools cannot be templated with rvalue references");

        friend ZippedLongest zip_longest<Container>(Container&&);

        template <typename C, typename... RC>
        friend class ZippedLongest;

        private:
            Container container;
            ZippedLongest(Container container)
                : container(std::forward<Container>(container))
            { }

        public:

            class Iterator {
                private:
                    using OptType = boost::optional<iterator_deref<Container>>;
                    iterator_type<Container> iter;
                    iterator_type<Container> end;
                public:
                    Iterator(
                            iterator_type<Container> it,
                            iterator_type<Container> in_end)
                        : iter{it},
                        end{in_end}
                    { }

                    Iterator& operator++() {
                        if (this->iter != this->end) {
                            ++this->iter;
                        }
                        return *this;
                    }

                    bool operator!=(const Iterator& other) const {
                        return this->iter != other.iter;
                    }

                    std::tuple<OptType> operator*() {
                        if (this->iter != this->end) {
                            return std::tuple<OptType>{OptType{*this->iter}};
                        }
                        return std::tuple<OptType>{OptType{}};
                    }
            };

            Iterator begin() {
                return {std::begin(this->container),
                    std::end(this->container)};
            }

            Iterator end() {
                return {std::end(this->container),
                    std::end(this->container)};
            }
    };

    template <typename... Containers>
    ZippedLongest<Containers...> zip_longest(Containers&&... containers) {
        return {std::forward<Containers>(containers)...};
    }
}

#endif // #ifndef ZIP_LONGEST_HPP_
