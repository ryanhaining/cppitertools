#ifndef POWERSET_HPP_
#define POWERSET_HPP_

#include "iterbase.hpp"
#include "combinations.hpp"

#include <vector>
#include <initializer_list>
#include <utility>
#include <iterator>

namespace iter {
    template <typename Container>
    class Powersetter {
        private:
            Container container;

        public:
            Powersetter(Container container)
                : container(std::forward<Container>(container))
            { }

            class Iterator {
                private:
                    std::size_t container_size;
                    std::size_t list_size = 0;
                    bool not_done = true;

                    using CombinatorType =
                        decltype(combinations(std::declval<Container&>(), 0));
                    std::vector<CombinatorType> combinators;
                    std::vector<iterator_type<CombinatorType>> inner_iters;
                public:
                    Iterator(Container& container)
                        : container_size{container.size()}
                    {
                        for (std::size_t i = 0; i <= container_size; ++i) {
                            combinators.push_back(combinations(container, i));
                            inner_iters.push_back(std::begin(combinators.back()));
                        }
                    }

                    Iterator& operator++() {
                        ++inner_iters[list_size];
                        if (!(inner_iters[list_size] != inner_iters[list_size])) {
                            ++list_size;
                        }
                        if (container_size < list_size) {
                            not_done = false;
                        }

                        return *this;
                    }

                    auto operator*() -> decltype(*inner_iters[0]) {
                        return *(inner_iters[list_size]);
                    }

                    bool operator != (const Iterator&) {
                        return not_done;
                    }
            }; 

            Iterator begin() {
                return {this->container};
            }

            Iterator end() {
                return {this->container};
            }
    };

    template <typename Container>
    Powersetter<Container> powerset(Container&& container) {
        return {std::forward<Container>(container)};
    }

    template <typename T>
    Powersetter<std::initializer_list<T>> powerset(
            std::initializer_list<T> il) {
        return {il};
    }
}
#endif // #ifndef POWERSET_HPP_
