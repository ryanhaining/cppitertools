#ifndef ITER_PERMUTATIONS_HPP_
#define ITER_PERMUTATIONS_HPP_
#include "iterbase.hpp"

#include <algorithm>
#include <initializer_list>
#include <vector>
#include <utility>
#include <iterator>

namespace iter {

    template <typename Container>
    class Permuter {
        private:
            Container container;

            using Permutable =
                std::vector<collection_item_type<Container>>;

        public:
            Permuter(Container&& in_container)
                : container(std::forward<Container>(in_container))
            { }

            class Iterator 
                : public std::iterator<std::input_iterator_tag, Permutable>
            {
                private:
                    constexpr static const int COMPLETE = -1;

                    Permutable working_set;
                    int steps{};

                public:
                    Iterator(iterator_type<Container> sub_iter,
                            iterator_type<Container> sub_end)
                        : steps{ sub_iter != sub_end ? 0 : COMPLETE }
                    {
                        // done like this instead of using vector ctor with
                        // two iterators because that causes a substitution
                        // failure when the iterator is minimal
                        while (sub_iter != sub_end) {
                            this->working_set.emplace_back(*sub_iter);
                            ++sub_iter;
                        }
                        std::sort(std::begin(working_set),
                                std::end(working_set));
                    }

                    Permutable& operator*() {
                        return working_set;
                    }

                    Iterator& operator++() {
                        ++this->steps;
                        if (!std::next_permutation(std::begin(working_set),
                                    std::end(working_set))) {
                            this->steps = COMPLETE;
                        }
                        return *this;
                    }

                    Iterator operator++(int) {
                        auto ret = *this;
                        ++*this;
                        return ret;
                    }

                    bool operator!=(const Iterator& other) const {
                        return !(*this == other);
                    }

                    bool operator==(const Iterator& other) const {
                        return this->steps == other.steps;
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

    template <typename Container>
    Permuter<Container> permutations(Container&& container) {
        return {std::forward<Container>(container)};
    }

    template <typename T>
    Permuter<std::initializer_list<T>> permutations(
            std::initializer_list<T> il) {
        return {std::move(il)};
    }

}

#endif
