#ifndef ITER_PERMUTATIONS_HPP_
#define ITER_PERMUTATIONS_HPP_
#include "iterbase.hpp"

#include <algorithm>
#include <initializer_list>
#include <vector>
#include <utility>


namespace iter {

    template <typename Container>
    class Permuter {
        private:
            Container container;

        public:
            Permuter(Container in_container)
                : container(in_container)
            { }

            class Iterator {
                private:
                    using Permutable =
                        std::vector<collection_item_type<Container>>;
                    Permutable working_set;
                    bool is_not_last = true;

                public:
                    Iterator(Container& c)
                    {
                        // done like this instead of using vector ctor with
                        // two iterators because that causes a substitution
                        // failure when the iterator is minimal
                        for (auto&& i : c) {
                            working_set.emplace_back(i);
                        }
                        std::sort(std::begin(working_set),
                                std::end(working_set));
                    }

                    Permutable& operator*() {
                        return working_set;
                    }

                    Iterator& operator++() {
                        is_not_last =
                            std::next_permutation(std::begin(working_set),
                                    std::end(working_set));
                        return *this;
                    }

                    bool operator!=(const Iterator&) const {
                        return is_not_last;
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
    Permuter<Container> permutations(Container&& container) {
        return {std::forward<Container>(container)};
    }

    template <typename T>
    Permuter<std::initializer_list<T>> permutations(
            std::initializer_list<T> il) {
        return {il};
    }

}

#endif // ITER_PERMUTATIONS_HPP_

