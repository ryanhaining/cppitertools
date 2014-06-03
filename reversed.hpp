#ifndef REVERSE_HPP__
#define REVERSE_HPP__

#include "iterbase.hpp"

#include <utility>
#include <iterator>

namespace iter {
    //Forward declarations of Reverser and reversed
    template <typename Container>
    class Reverser;

    template <typename Container>
    Reverser<Container> reversed(Container&&);


    template <typename Container>
    class Reverser {
        private:
            Container container;
            // The reversed function is the only thing allowed to create a
            // Reverser
            friend Reverser reversed<Container>(Container&&);
            
            // Value constructor for use only in the reversed function
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
    //
    // specialization for statically allocated arrays
    // this involves some tricks
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
                    T *stop;
                    T *dummy_end;
                public:
                    // iter should be the last element in the array
                    // stop should be the first element
                    // dummy should be what iter is set to when complete
                    //    the implementation below sets the dummy to one-past-
                    //    the end, since that's the only non-nullptr value that
                    //    the pointer can be set to that is also not a part
                    //    of the actual array
                    Iterator (T *iter, T *stop, T *dummy)
                        : sub_iter{iter},
                        stop{stop},
                        dummy_end{dummy}
                    { } 

                    auto operator*() -> decltype(*array) {
                        return *this->sub_iter;
                    }

                    Iterator& operator++() { 
                        if (this->sub_iter == this->stop) {
                            this->sub_iter = this->dummy_end;
                        } else {
                            // decrementing the pointer is going forwards
                            // in the reversed direction
                            --this->sub_iter;
                        }
                        return *this;
                    }

                    bool operator!=(const Iterator& other) const {
                        return this->sub_iter != other.sub_iter;
                    }
            };

            T *dummy_end() const {
                return this->array + N;
            }

            Iterator begin() {
                return {this->array + N - 1,
                        this->array,
                        this->dummy_end()};
            }

            Iterator end() {
            return {this->dummy_end(), this->dummy_end(), this->dummy_end()};
            }

    };

    template <typename T, std::size_t N>
    Reverser<T[N]> reversed(T (&array)[N]) {
        return {array};
    }

}

#endif //REVERSE_HPP__
