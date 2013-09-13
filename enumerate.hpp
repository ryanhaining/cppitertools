#ifndef ENUMERABLE__H__
#define ENUMERABLE__H__

#include <utility>

namespace iter {

    template <typename Container>
    class Enumerable {
        // The only thing allowed to directly instantiate an Enumerable is
        // the enumerate function
        private:
            Container & container;
            
            using contained_iter_type =
                decltype(((Container*)nullptr)->begin());
            using contained_iter_ret =
                decltype(((contained_iter_type*)nullptr)->operator*());
        public:
            // "yielded" by the Enumerable::Iterator.  Has a .index, and a 
            // .element referencing the value yielded by the subiterator
            class IterYield {
                public:
                    std::size_t index;
                    contained_iter_ret & element;
                    IterYield(std::size_t i, contained_iter_ret & elem) : 
                        index(i),
                        element(elem) { }
            };

            //  Holds an iterator of the contained type and a size_t for the
            //  index.  Each call to ++ increments both of these data members.
            //  Each dereference returns an IterYield.
            class Iterator {
                private:
                    contained_iter_type sub_iter;
                    std::size_t index;
                public:
                    Iterator (contained_iter_type si) :
                        sub_iter(si),
                        index(0) { } 

                    IterYield operator*() const {
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

            Enumerable(Container & container) : container(container) { }
            Enumerable () = delete;
            Enumerable & operator=(const Enumerable &) = delete;

            Iterator begin() const {
                return Iterator(std::move(this->container.begin()));
            }

            Iterator end() const {
                return Iterator(std::move(this->container.end()));
            }

    };

    // Helper function to instantiate an enumerable
    template <typename Container>
    Enumerable<Container> enumerate(Container & container) {
        return Enumerable<Container>(container);
    }
}


#endif //ifndef ENUMERABLE__H__
