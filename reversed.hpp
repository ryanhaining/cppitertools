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

    // Helper function to instantiate an Filter
    template <typename Container>
    Reverser<Container> reversed(Container&& container) {
        return {std::forward<Container>(container)};
    }
}

#endif //REVERSE_HPP__
