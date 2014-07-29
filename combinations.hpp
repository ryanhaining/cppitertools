#ifndef COMBINATIONS_HPP_
#define COMBINATIONS_HPP_

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
        public:
            Combinator(Container in_container, std::size_t in_length)
                : container(std::forward<Container>(in_container)),
                length{in_length}
            { }


        class Iterator {
            private:
                Container& items;
                std::vector<iterator_type<Container>> indicies;
                bool not_done = true;

                using item_t = typename 
                    std::remove_const<
                        typename std::remove_reference<
                            iterator_deref<Container>>::type>::type;

            public:
                Iterator(Container& i, size_t N)
                    : items(i),
                    indicies(N)
                {
                    if (N == 0) {
                        not_done = false;
                        return;
                    }
                    size_t inc = 0;
                    for (auto& iter : this->indicies) {
                        if (std::begin(items) + inc != std::end(items)) {
                            iter = std::begin(items)+inc;
                            ++inc;
                        } else {
                            not_done = false;
                            break;
                        }
                    }
                }

                std::vector<item_t> operator*() {
                    std::vector<item_t> values;
                    for (auto i : indicies) {
                        values.push_back(*i);
                    }
                    return values;
                }


                Iterator& operator++() {
                    for (auto iter = indicies.rbegin();
                            iter != indicies.rend();
                            ++iter) {
                        ++(*iter);
                        //what we have to check here is if the distance between
                        //the index and the end of indicies is >= the distance
                        //between the item and end of item
                        if ((*iter + std::distance(indicies.rbegin(),iter)) ==
                                std::end(items)) {
                            if ( (iter + 1) != indicies.rend()) {
                                size_t inc = 1;
                                for (auto down = iter;
                                        down != indicies.rbegin()-1;
                                        --down) {
                                    (*down) = (*(iter + 1)) + 1 + inc;
                                    /*if (*down == items.cend()) {
                                        iter = iter + 1;
                                    }*/
                                    ++inc;
                                }
                            } else {
                                not_done = false;
                                break;
                            }
                        } else {
                            break; 
                        }
                        //we break because none of the rest of the items need
                        //to be incremented
                    }
                    return *this;
                }

                bool operator !=(const Iterator&)
                {
                    //because of the way this is done you have to start from
                    //the begining of the range and end at the end, you could
                    //break in the middle of the loop though, it's not
                    //different from the way that python's works
                    return not_done;
                }
        };

        Iterator begin() {
            return {this->container, this->length};
        }
        
        Iterator end() {
            return {this->container, this->length};
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
        return {il, length};
    }
}
#endif //#ifndef COMBINATIONS_HPP_
