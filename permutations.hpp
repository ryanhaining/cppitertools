#ifndef PERMUTATIONS_HPP
#define PERMUTATIONS_HPP
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
            // always copy, never move
            Permuter(Container in_container)
                : container(in_container)
            {
                    std::sort(std::begin(this->container),
                        std::end(this->container));
            }

            class Iterator {
                private:
                    using Permutable =
                        std::vector<collection_item_type<Container>>;
                    Permutable working_set;
                    bool is_not_last = true;

                public:
                    Iterator(Container& c)
                        : working_set{std::begin(c), std::end(c)}
                    { }

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

    // NOTE unlike other itertools, this one copies the input container
    // rather than taking a universal ref
    template <typename Container>
    Permuter<Container> permutations(Container&& container) {
        return {std::forward<Container>(container)};
    }

    //since initializer_list doesn't have bidir iters this is a hack
    //to get it to work by using a vector in its place
    template <typename T>
    Permuter<std::vector<T>> permutations(std::initializer_list<T> il) {
        std::vector<T> vec = il;
        return {std::move(vec)};
    }

}

#endif //PERMUTATIONS_HPP

