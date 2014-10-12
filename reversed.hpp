#ifndef ITER_REVERSE_HPP_
#define ITER_REVERSE_HPP_

#include "iterbase.hpp"

#include <utility>
#include <iterator>

namespace iter {
    template <typename Container>
    class Reverser;

    template <typename Container>
    Reverser<Container> reversed(Container&&);


    template <typename Container>
    class Reverser {
        private:
            Container container;
            friend Reverser reversed<Container>(Container&&);
            
            Reverser(Container container)
                : container(std::forward<Container>(container))
            { }
            Reverser() = delete;
            Reverser& operator=(const Reverser&) = delete;

        public:
            Reverser(const Reverser&) = default;
            class Iterator {
                private:
                    reverse_iterator_type<Container> sub_iter;
                public:
                    Iterator (reverse_iterator_type<Container> iter)
                        : sub_iter{iter}
                    { } 

                    reverse_iterator_deref<Container> operator*() {
                        return *this->sub_iter;
                    }

                    Iterator& operator++() { 
                        ++this->sub_iter;
                        return *this;
                    }

                    bool operator!=(const Iterator& other) const {
                        return this->sub_iter != other.sub_iter;
                    }
            };

            Iterator begin() {
                return {this->container.rbegin()};
            }

            Iterator end() {
                return {this->container.rend()};
            }

    };

    // Helper function to instantiate a Reverser
    template <typename Container>
    Reverser<Container> reversed(Container&& container) {
        return {std::forward<Container>(container)};
    }

    //
    // specialization for statically allocated arrays
    //
    template <typename T, std::size_t N>
    Reverser<T[N]> reversed(T (&)[N]);

    template <typename T, std::size_t N>
    class Reverser<T[N]> {
        private:
            T *array;
            // The reversed function is the only thing allowed to create a
            // Reverser
            friend Reverser reversed<T, N>(T (&)[N]);
            
            // Value constructor for use only in the reversed function
            Reverser(T *array)
                : array{array}
            { }
            Reverser() = delete;
            Reverser& operator=(const Reverser&) = delete;

        public:
            Reverser(const Reverser&) = default;
            class Iterator {
                private:
                    T *sub_iter;
                public:
                    Iterator (T *iter)
                        : sub_iter{iter}
                    { } 

                    auto operator*() -> decltype(*array) {
                        return *(this->sub_iter - 1);
                    }

                    Iterator& operator++() { 
                        --this->sub_iter;
                        return *this;
                    }

                    bool operator!=(const Iterator& other) const {
                        return this->sub_iter != other.sub_iter;
                    }
            };

            Iterator begin() {
                return {this->array + N};
            }

            Iterator end() {
            return {this->array};
            }

    };

    template <typename T, std::size_t N>
    Reverser<T[N]> reversed(T (&array)[N]) {
        return {array};
    }

}

#endif //ITER_REVERSE_HPP_
