#ifndef ITER_ENUMERATE_H_
#define ITER_ENUMERATE_H_

#include "iterbase.hpp"

#include <utility>
#include <iterator>
#include <functional>
#include <initializer_list>
#include <type_traits>

namespace iter {

    //Forward declarations of Enumerable and enumerate
    template <typename Container>
    class Enumerable;

    template <typename Container>
    Enumerable<Container> enumerate(Container&&);

    template <typename T>
    Enumerable<std::initializer_list<T>> enumerate(std::initializer_list<T>);

    template <typename Container>
    class Enumerable {
        private:
            Container container;

            // The only thing allowed to directly instantiate an Enumerable is
            // the enumerate function
            friend Enumerable enumerate<Container>(Container&&);
            template <typename T>
            friend Enumerable<std::initializer_list<T>> enumerate(
                    std::initializer_list<T>);

            // for IterYield
            using BasePair = std::pair<std::size_t, iterator_deref<Container>>;

            // Value constructor for use only in the enumerate function
            Enumerable(Container&& in_container)
                : container(std::forward<Container>(in_container))
            { }

        public:
            // "yielded" by the Enumerable::Iterator.  Has a .index, and a
            // .element referencing the value yielded by the subiterator
            class IterYield : public BasePair {
                public:
                    using BasePair::BasePair;
                    decltype(BasePair::first)& index = BasePair::first;
                    decltype(BasePair::second)& element = BasePair::second;
            };

            //  Holds an iterator of the contained type and a size_t for the
            //  index.  Each call to ++ increments both of these data members.
            //  Each dereference returns an IterYield.
            class Iterator :
                public std::iterator<std::input_iterator_tag, IterYield>
            {
                private:
                    iterator_type<Container> sub_iter;
                    std::size_t index;
                public:
                    Iterator(iterator_type<Container>&& si)
                        : sub_iter{std::move(si)},
                        index{0}
                    { }

                    IterYield operator*() {
                        return {this->index, *this->sub_iter};
                    }

                    Iterator& operator++() {
                        ++this->sub_iter;
                        ++this->index;
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
                return {std::begin(this->container)};
            }

            Iterator end() {
                return {std::end(this->container)};
            }

    };

    template <typename Container>
    Enumerable<Container> enumerate(Container&& container) {
        return {std::forward<Container>(container)};
    }

    template <typename T>
    Enumerable<std::initializer_list<T>> enumerate(
            std::initializer_list<T> il)
    {
        return {std::move(il)};
    }
}

#endif
