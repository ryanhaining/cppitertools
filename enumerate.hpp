#ifndef ENUMERABLE__H__
#define ENUMERABLE__H__

#include "iterbase.hpp"

#include <utility>
#include <iterator>
#include <functional>
#include <initializer_list>


// enumerate functionality for python-style for-each enumerate loops
// for (auto e : enumerate(vec)) {
//     std::cout << e.index
//               << ": "
//               << e.element
//               << '\n';
// }


namespace iter {

    //Forward declarations of Enumerable and enumerate
    template <typename Container>
    class Enumerable;

    template <typename T>
    Enumerable<std::initializer_list<T>> enumerate(
            std::initializer_list<T> &&);

    template <typename Container>
    Enumerable<Container> enumerate(Container &&);

    template <typename Container>
    class Enumerable {
        private:
            Container & container;

            // The only thing allowed to directly instantiate an Enumerable is
            // the enumerate function
            friend Enumerable enumerate<Container>(Container &&);
            template <typename T>
            friend Enumerable<std::initializer_list<T>> enumerate(
                    std::initializer_list<T> &&);

            Enumerable(Container && container) : container{container} { }
            
        public:
            // Value constructor for use only in the enumerate function
            Enumerable () = delete;
            Enumerable & operator=(const Enumerable &) = delete;

            Enumerable(const Enumerable &) = default;

            // "yielded" by the Enumerable::Iterator.  Has a .index, and a 
            // .element referencing the value yielded by the subiterator
            class IterYield {
                public:
                    std::size_t index;
                    iterator_deref<Container> element;
                    IterYield(std::size_t i, iterator_deref<Container> elem)
                        : index{i},
                        element{elem}
                    { }
            };

            //  Holds an iterator of the contained type and a size_t for the
            //  index.  Each call to ++ increments both of these data members.
            //  Each dereference returns an IterYield.
            class Iterator {
                private:
                    iterator_type<Container> sub_iter;
                    std::size_t index;
                public:
                    Iterator (iterator_type<Container> si)
                        : sub_iter{si},
                        index{0}
                    { } 

                    const IterYield operator*() const {
                        return IterYield(this->index, *this->sub_iter);
                    }
			  IterYield operator*() {
				return IterYield(this->index, *this->sub_iter);
			  }

                    Iterator & operator++() { 
                        ++this->sub_iter;
                        ++this->index;
                        return *this;
                    }

                    bool operator!=(const Iterator & other) const {
                        return this->sub_iter != other.sub_iter;
                    }
            };

            Iterator begin() const {
                return {std::begin(this->container)};
            }

            Iterator end() const {
                return {std::end(this->container)};
            }

    };

    // Helper function to instantiate an Enumerable
    template <typename Container>
    Enumerable<Container> enumerate(Container && container) {
        return {std::forward<Container>(container)};
    }

    template <typename T>
    Enumerable<std::initializer_list<T>> enumerate(
            std::initializer_list<T> && il)
    {
        return {std::move(il)};
    }

}

#endif //ifndef ENUMERABLE__H__
