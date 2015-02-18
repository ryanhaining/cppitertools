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
            
            Reverser(Container&& container)
                : container(std::forward<Container>(container))
            { }

        public:
            class Iterator : public std::iterator<
                         std::input_iterator_tag,
                         iterator_traits_deref<Container>>
            {
                private:
                    reverse_iterator_type<Container> sub_iter;
                public:
                    Iterator (reverse_iterator_type<Container>&& iter)
                        : sub_iter{std::move(iter)}
                    { } 

                    reverse_iterator_deref<Container> operator*() {
                        return *this->sub_iter;
                    }

                    Iterator& operator++() { 
                        ++this->sub_iter;
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
                return {this->container.rbegin()};
            }

            Iterator end() {
                return {this->container.rend()};
            }

    };

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
            friend Reverser reversed<T, N>(T (&)[N]);
            
            // Value constructor for use only in the reversed function
            Reverser(T *array)
                : array{array}
            { }

        public:
            Reverser(const Reverser&) = default;
            class Iterator : public std::iterator<std::input_iterator_tag, T>
            {
                private:
                    T *sub_iter;
                public:
                    Iterator (T *iter)
                        : sub_iter{iter}
                    { } 

                    iterator_deref<T[N]> operator*() {
                        return *(this->sub_iter - 1);
                    }

                    Iterator& operator++() { 
                        --this->sub_iter;
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

#endif
