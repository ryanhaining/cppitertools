#ifndef GROUPER_HPP_
#define GROUPER_HPP_

#include "iterbase.hpp"

#include <vector>
#include <algorithm>
#include <type_traits>
#include <functional>
#include <utility>
#include <iterator>
#include <initializer_list>

namespace iter {
    template <typename Container>
    class Grouper;

    template <typename Container>
    Grouper<Container> grouper(Container&&, std::size_t);

    template <typename T>
    Grouper<std::initializer_list<T>> grouper(
            std::initializer_list<T>, std::size_t);

    template <typename Container>
    class Grouper {
        private:
            Container container;
            std::size_t group_size;

            Grouper(Container c, std::size_t sz)
                : container(std::forward<Container>(c)),
                group_size{sz}
            { }

            friend Grouper grouper<Container>(Container&&, std::size_t);
            template <typename T>
            friend Grouper<std::initializer_list<T>> grouper(
                    std::initializer_list<T>, std::size_t);

        public:
            class Iterator {
                private:
                    Container& container;
                    std::vector<iterator_type<Container>> group;
                    std::size_t group_size = 0;
                    bool not_done = true;

                    using Deref_type =
                        std::vector<
                            std::reference_wrapper<
                                typename std::remove_reference<
                                    iterator_deref<Container>>::type>>;


                public: 
                    Iterator(Container& c, std::size_t s)
                        : container(c),
                        group_size(s) 
                    {
                        // if the group size is 0 or the container is empty produce
                        // nothing
                        if (this->group_size == 0
                                || (!(std::begin(this->container)
                                        != std::end(this->container)))) {
                            this->not_done = false;
                            return;
                        }
                        std::size_t i = 0;
                        for (auto iter = std::begin(container);
                                i < group_size;
                                ++i, ++iter) {
                            group.push_back(iter);
                        }
                    }

                    //seems like conclassor is same as sliding_window_iter
                    Iterator(Container& c)
                        : container(c)
                    {
                        //creates the end iterator
                        group.push_back(std::end(container));
                    }

                    //plan to conditionally check for existence of +=
                    Iterator & operator++() {
                        for (auto & iter : this->group) {
                            std::advance(iter,this->group_size);
                        }
                        return *this;
                    }

                    bool operator!=(const Iterator &) const {
                        return this->not_done;
                    }

                    Deref_type operator*() {
                        Deref_type vec;
                        for (auto i : this->group) {
                            if(!(i != std::end(this->container))) {
                                this->not_done = false;
                                break;
                            } 
                            //if the group is at the end the vector will be smaller 
                            else { 
                                vec.push_back(*i);
                            }
                        }
                        return vec;
                    }
            };

            Iterator begin() {
                return {this->container, group_size};
            }

            Iterator end() {
                return {this->container};
            }
    };

    template <typename Container>
    Grouper<Container> grouper(Container&& container, std::size_t group_size) {
        return {std::forward<Container>(container), group_size};
    }

    template <typename T>
    Grouper<std::initializer_list<T>> grouper(
            std::initializer_list<T> il, std::size_t group_size) {
        return {il, group_size};
    }
}
#endif // #ifndef GROUPER_HPP_
