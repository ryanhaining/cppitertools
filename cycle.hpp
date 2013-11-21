#ifndef CYCLE__H__
#define CYCLE__H__

#include <iterbase.hpp>

#include <utility>
#include <iterator>
#include <initializer_list>

namespace iter {

    //Forward declarations of Cycle and cycle
    template <typename Container>
    class Cycle;

    template <typename Container>
    Cycle<Container> cycle(Container &&);

    template <typename T>
    Cycle<std::initializer_list<T>> cycle(
            std::initializer_list<T> &&);

    template <typename Container>
    class Cycle : public IterBase<Container>{
        private:
            // The cycle function is the only thing allowed to create a Cycle
            friend Cycle cycle<Container>(Container &&);
            template <typename T>
            friend Cycle<std::initializer_list<T>> cycle(
                    std::initializer_list<T> &&);

            using typename IterBase<Container>::contained_iter_type;

            using typename IterBase<Container>::contained_iter_ret;

            Container & container;
            
            // Value constructor for use only in the cycle function
            Cycle(Container && container) : container(container) { }
            Cycle () = delete;
            Cycle & operator=(const Cycle &) = delete;

        public:
            Cycle(const Cycle &) = default;
            class Iterator {
                private:
                    contained_iter_type sub_iter;
                    const contained_iter_type begin;
                    const contained_iter_type end;
                public:
                    Iterator (contained_iter_type iter,
                            contained_iter_type end) :
                        sub_iter(iter),
                        begin(iter),
                        end(end)
                    { } 

                    contained_iter_ret operator*() const {
                        return *this->sub_iter;
                    }

                    Iterator & operator++() { 
                        ++this->sub_iter;
                        // reset to beginning upon reaching the end
                        if (!(this->sub_iter != this->end)) {
                            this->sub_iter = this->begin;
                        }
                        return *this;
                    }

                    constexpr bool operator!=(const Iterator &) const {
                        return true;
                    }
            };

            Iterator begin() const {
                return Iterator(std::begin(this->container),
                        std::end(this->container));
            }

            Iterator end() const {
                return Iterator(std::end(this->container),
                    std::end(this->container));
            }

    };

    // Helper function to instantiate an Filter
    template <typename Container>
    Cycle<Container> cycle(Container && container) {
        return Cycle<Container>(std::forward<Container>(container));
    }

    template <typename T>
    Cycle<std::initializer_list<T>> cycle(std::initializer_list<T> && il)
    {
        return Cycle<std::initializer_list<T>>(
                std::forward<std::initializer_list<T>>(il));
    }
}

#endif //ifndef CYCLE__H__
