#ifndef ITER_CYCLE_H_
#define ITER_CYCLE_H_

#include "iterbase.hpp"

#include <utility>
#include <iterator>
#include <initializer_list>

namespace iter {

    template <typename Container>
    class Cycle;

    template <typename Container>
    Cycle<Container> cycle(Container&&);

    template <typename T>
    Cycle<std::initializer_list<T>> cycle(std::initializer_list<T>);

    template <typename Container>
    class Cycle {
        private:
            friend Cycle cycle<Container>(Container&&);
            template <typename T>
            friend Cycle<std::initializer_list<T>> cycle(
                    std::initializer_list<T>);

            Container container;
            
            Cycle(Container&& container)
                : container(std::forward<Container>(container))
            { }

        public:
            class Iterator 
                : public std::iterator<std::input_iterator_tag,
                            iterator_traits_deref<Container>>
            {
                private:
                    using iter_type = iterator_type<Container>;
                    iterator_type<Container> sub_iter;
                    iterator_type<Container> begin;
                    iterator_type<Container> end;
                public:
                    Iterator (const iterator_type<Container>& iter,
                            iterator_type<Container>&& end)
                        : sub_iter{iter},
                        begin{iter},
                        end{std::move(end)}
                    { } 

                    iterator_deref<Container> operator*() {
                        return *this->sub_iter;
                    }

                    Iterator& operator++() { 
                        ++this->sub_iter;
                        // reset to beginning upon reaching the end
                        if (!(this->sub_iter != this->end)) {
                            this->sub_iter = this->begin;
                        }
                        return *this;
                    }

                    Iterator operator++(int) {
                        auto ret = *this;
                        ++*this;
                        return ret;
                    }

                    bool operator!=(const Iterator& other) const {
                        return this->sub_iter != other.sub_iter;
                    }

                    bool operator==(const Iterator& other) const {
                        return !(*this != other);
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
    Cycle<Container> cycle(Container&& container) {
        return {std::forward<Container>(container)};
    }

    template <typename T>
    Cycle<std::initializer_list<T>> cycle(std::initializer_list<T> il)
    {
        return {std::move(il)};
    }
}

#endif
