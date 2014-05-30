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

    template <typename Container>
    Enumerable<Container&> enumerate(Container&);

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
            template <typename C>
            friend Enumerable<C&> enumerate(C&);
            template <typename C>
            friend Enumerable<C> enumerate(C&&);
            template <typename T>
            friend Enumerable<std::initializer_list<T>> enumerate(
                    std::initializer_list<T>);
    
            Enumerable(Container container)
                : container(std::forward<Container>(container))
            { }
            
        public:
            // Value constructor for use only in the enumerate function
            Enumerable() = delete;
            Enumerable& operator=(const Enumerable&) = delete;
            Enumerable(const Enumerable&) = delete;
            Enumerable& operator=(Enumerable&&) = delete;

            // movable only
            Enumerable(Enumerable&&) = default;
            ~Enumerable() = default;

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

                    IterYield operator*() const {
                        return IterYield(this->index, *this->sub_iter);
                    }

                    Iterator& operator++() { 
                        ++this->sub_iter;
                        ++this->index;
                        return *this;
                    }

                    bool operator!=(const Iterator& other) const {
                        return this->sub_iter != other.sub_iter;
                    }
            };

            Iterator begin() {
                return {std::begin(this->container)};
            }

            Iterator end() {
                return {std::end(this->container)};
            }

    };

    // any lvalues passed will go here, instantiating Enumerabe<Container&>
    // this will result in enumerate iterating over a reference to the
    // provided value, as is the usual intention
    template <typename Container>
    Enumerable<Container&> enumerate(Container& container) {
        return {container};
    }

    // any rvalue passed will go here, instantiating Enumerabel<Container>
    // instead of binding a reference, the Enumerable ctor will move construct
    // the temporary `container` as a data member
    template <typename Container>
    Enumerable<Container> enumerate(Container&& container) {
        return {std::move(container)};
    }

    // for initializer lists.  copy constructs the list into the Enumerable
    template <typename T>
    Enumerable<std::initializer_list<T>> enumerate(
            std::initializer_list<T> il)
    {
        return {il};
    }

}

#endif //ifndef ENUMERABLE__H__
