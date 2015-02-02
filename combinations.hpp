#ifndef ITER_COMBINATIONS_HPP_
#define ITER_COMBINATIONS_HPP_

#include "iterbase.hpp"

#include <vector>
#include <type_traits>
#include <iterator>
#include <initializer_list>

namespace iter {
    template <typename Container>
    class Combinator;

    template <typename Container>
    Combinator<Container> combinations(Container&&, std::size_t);

    template <typename T>
    Combinator<std::initializer_list<T>> combinations(
            std::initializer_list<T>, std::size_t);

    template <typename Container>
    class Combinator {
        private:
            Container container;
            std::size_t length;

            friend Combinator combinations<Container>(Container&&,std::size_t);
            template <typename T>
            friend Combinator<std::initializer_list<T>> combinations(
                    std::initializer_list<T>, std::size_t);

            Combinator(Container&& in_container, std::size_t in_length)
                : container(std::forward<Container>(in_container)),
                length{in_length}
            { }

            using CombIteratorDeref =
                std::vector<collection_item_type<Container>>;
        public:

        class Iterator :
            public std::iterator<std::input_iterator_tag, CombIteratorDeref>
        {
            private:
                constexpr static const int COMPLETE = -1;
                typename std::remove_reference<Container>::type *container_p;
                std::vector<iterator_type<Container>> indicies;
                CombIteratorDeref working_set;
                int steps{};

                void compute_working_set() {
                    this->working_set.clear();
                    for (auto&& i : this->indicies) {
                        this->working_set.emplace_back(*i);
                    }
                }

            public:
                Iterator(Container& in_container, std::size_t n)
                    : container_p{&in_container},
                    indicies{n}
                {
                    if (n == 0) {
                        this->steps = COMPLETE;
                        return;
                    }
                    size_t inc = 0;
                    for (auto& iter : this->indicies) {
                        auto it = std::begin(*this->container_p);
                        dumb_advance(it, std::end(*this->container_p), inc);
                        if (it != std::end(*this->container_p)) {
                            iter = it;
                            ++inc;
                        } else {
                            this->steps = COMPLETE;
                            break;
                        }
                    }
                    if (this->steps != COMPLETE) {
                        this->compute_working_set();
                    }
                }

                CombIteratorDeref& operator*() {
                    return this->working_set;
                }


                Iterator& operator++() {
                    for (auto iter = indicies.rbegin();
                            iter != indicies.rend();
                            ++iter) {
                        ++(*iter);

                        //what we have to check here is if the distance between
                        //the index and the end of indicies is >= the distance
                        //between the item and end of item
                        auto dist = std::distance(
                                this->indicies.rbegin(),iter);

                        if (!(dumb_next(*iter, dist) !=
                                std::end(*this->container_p))) {
                            if ( (iter + 1) != indicies.rend()) {
                                size_t inc = 1;
                                for (auto down = iter;
                                        down != indicies.rbegin()-1;
                                        --down) {
                                    (*down) = dumb_next(*(iter + 1), 1 + inc);
                                    ++inc;
                                }
                            } else {
                                this->steps = COMPLETE;
                                break;
                            }
                        } else {
                            break; 
                        }
                        //we break because none of the rest of the items need
                        //to be incremented
                    }
                    if (this->steps != COMPLETE) {
                        ++this->steps;
                        this->compute_working_set();
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
            return {this->container, this->length};
        }
        
        Iterator end() {
            return {this->container, 0};
        }
    };

    template <typename Container>
    Combinator<Container> combinations(
            Container&& container, std::size_t length) {
        return {std::forward<Container>(container), length};
    }

    template <typename T>
    Combinator<std::initializer_list<T>> combinations(
            std::initializer_list<T> il, std::size_t length) {
        return {std::move(il), length};
    }
}
#endif
