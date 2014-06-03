#ifndef CYCLE__H__
#define CYCLE__H__

#include "iterbase.hpp"

#include <utility>
#include <iterator>
#include <initializer_list>

namespace iter {

    //Forward declarations of Cycle and cycle
    template <typename Container>
    class Cycle;

    template <typename Container>
    Cycle<Container> cycle(Container&&);

    template <typename T>
    Cycle<std::initializer_list<T>> cycle(std::initializer_list<T>);

    template <typename Container>
    class Cycle {
        private:
            // The cycle function is the only thing allowed to create a Cycle
            friend Cycle cycle<Container>(Container&&);
            template <typename T>
            friend Cycle<std::initializer_list<T>> cycle(
                    std::initializer_list<T>);

            Container container;
            
            // Value constructor for use only in the cycle function
            Cycle(Container container)
                : container(std::forward<Container>(container))
            { }
            Cycle() = delete;
            Cycle& operator=(const Cycle&) = delete;

        public:
            Cycle(const Cycle&) = default;
            class Iterator {
                private:
                    using iter_type = iterator_type<Container>;
                    iterator_type<Container> sub_iter;
                    const iterator_type<Container> begin;
                    const iterator_type<Container> end;
                public:
                    Iterator (iterator_type<Container> iter,
                            iterator_type<Container> end)
                        : sub_iter{iter},
                        begin{iter},
                        end{end}
                    { } 

                    iterator_deref<Container> operator*() const {
                        return *this->sub_iter;
                    }

                    Iterator& operator++() { 
                        ++this->sub_iter;
                        // reset to beginning upon reaching the end
                        if (!(this->sub_iter != this->end)) {
                            // explicit destruction with placement new in order
                            // to support iterators with no operator=
                            this->sub_iter.~iter_type();
                            new(&this->sub_iter) iterator_type<Container>(
                                    this->begin);
                        }
                        return *this;
                    }

                    constexpr bool operator!=(const Iterator&) const {
                        return true;
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

    // Helper function to instantiate a Cycle
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

#endif //ifndef CYCLE__H__
