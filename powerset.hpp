#ifndef POWERSET_HPP_
#define POWERSET_HPP_

#include "iterbase.hpp"
#include "combinations.hpp"
#include "enumerate.hpp"

#include <cassert>
#include <vector>
#include <initializer_list>
#include <utility>
#include <iterator>

namespace iter {
    template <typename Container,
              typename CombinatorType=
                  decltype(combinations(std::declval<Container&>(), 0))>
    class Powersetter {
        private:
            Container container;

            std::vector<CombinatorType> combinators;
        public:
            Powersetter(Container in_container)
                : container(std::forward<Container>(in_container))
            {
                combinators.push_back(combinations(this->container, 0));
                std::size_t i = 1;
                for (auto iter = std::begin(this->container),
                            end = std::end(this->container);
                        iter != end;
                        ++iter, ++i) {
                    combinators.push_back(combinations(this->container, i));
                }
            }

            class Iterator {
                private:
                    std::size_t container_size;
                    std::size_t list_size = 0;
                    bool not_done = true;

                    std::vector<CombinatorType>& combinators;
                    std::vector<iterator_type<CombinatorType>> inner_iters;
                public:
                    Iterator(std::vector<CombinatorType>& combs)
                        : container_size{combs.size() - 1},
                        combinators(combs)
                    {
                        for (auto& comb : combinators) {
                            inner_iters.push_back(std::begin(comb));
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
                return {this->combinators};
            }

            Iterator end() {
                return {this->combinators};
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
