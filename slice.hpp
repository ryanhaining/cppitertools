#ifndef SLICE_HPP
#define SLICE_HPP

#include "iterbase.hpp"

#include <iterator>
#include <type_traits>
#include <initializer_list>


namespace iter {

    //Forward declarations of Slice and slice
    //template <typename Container, typename DifferenceType>
    //class Slice;

    //template <typename T>
    //Slice<std::initializer_list<T>> slice( std::initializer_list<T>);

    //template <typename Container, typename DifferenceType>
    //Slice<Container> slice(Container &&);
    template <typename T>
    class has_size
    {
        typedef char one;
        typedef long two;

        template <typename C> static one test( decltype(&C::size) ) ;
        template <typename C> static two test(...);


        public:
        enum { value = sizeof(test<T>(0)) == sizeof(char) };
    };
    template <typename Container>
    typename std::enable_if<has_size<Container>::value, std::size_t>::type 
    size(const Container& container) {
        return container.size();
    }

    template <typename Container>
    typename std::enable_if<!has_size<Container>::value, std::size_t>::type 
    size(const Container& container) {
        return std::distance(std::begin(container), std::end(container));
    }

    template <typename T, std::size_t N>
    std::size_t size(const T (&)[N]) {
        return N;
    }



    template <typename Container, typename DifferenceType>
    class Slice {
        private:
            Container container;
            DifferenceType start;
            DifferenceType stop;
            DifferenceType step;

            // The only thing allowed to directly instantiate an Slice is
            // the slice function
            //friend Slice slice<Container, DifferenceType>(Container &&);
            //template <typename T>
            //friend Slice<std::initializer_list<T>> slice(std::initializer_list<T>);
        public:
            Slice(Container in_container, DifferenceType start,
                  DifferenceType stop, DifferenceType step)
                : container(std::forward<Container>(in_container)),
                start{start},
                stop{stop},
                step{step}
            { 
                // sets stop = start if the range is empty
                if ((start < stop && step <=0) ||
                        (start > stop && step >=0)){
                    this->stop = start;
                } 
                if (this->stop > static_cast<DifferenceType>(
                            size(this->container))) {
                    this->stop = static_cast<DifferenceType>(size(
                                this->container));
                }
                if (this->start < 0) {
                    this->start = 0; 
                }
            }

            Slice() = delete;
            Slice& operator=(const Slice&) = delete;

            Slice(const Slice &) = default;


            class Iterator {
                private:
                    iterator_type<Container> sub_iter;
                    DifferenceType current;
                    const DifferenceType stop;
                    const DifferenceType step;

                public:
                    Iterator (iterator_type<Container> si, DifferenceType start,
                            DifferenceType stop, DifferenceType step)
                        : sub_iter{si},
                        current{start},
                        stop{stop},
                        step{step}
                    { }

                    iterator_deref<Container> operator*() const {
                        return *this->sub_iter;
                    }

                    Iterator& operator++() { 
                        std::advance(this->sub_iter, this->step);
                        this->current += this->step;
                        return *this;
                    }

                    bool operator!=(const Iterator &) const {
                        return (this->step > 0 && this->current < this->stop)||
                            (this->step < 0 && this->current > this->stop);
                    }
            };

            Iterator begin() {
                return {std::next(std::begin(this->container), this->start),
                        this->start, this->stop, this->step};
            }

            Iterator end() {
                return {std::next(std::begin(this->container), this->stop),
                        this->stop, this->stop, this->step};
            }

    };

    // Helper function to instantiate a Slice
    template <typename Container, typename DifferenceType>
    Slice<Container, DifferenceType> slice(
            Container&& container,
            DifferenceType start, DifferenceType stop, DifferenceType step=1) {
        return {std::forward<Container>(container), start, stop, step};
    }

    //only give the end as an arg and assume step  is 1 and begin is 0
    template <typename Container, typename DifferenceType>
    Slice<Container, DifferenceType> slice(
            Container&& container, DifferenceType stop) {
        return {std::forward<Container>(container), 0, stop, 1};
    }

    template <typename T, typename DifferenceType>
    Slice<std::initializer_list<T>, DifferenceType> slice(
            std::initializer_list<T> il, DifferenceType start,
            DifferenceType stop, DifferenceType step=1) {
        return {il, start, stop, step};
    }

    template <typename T, typename DifferenceType>
    Slice<std::initializer_list<T>, DifferenceType> slice(
            std::initializer_list<T> il, DifferenceType stop) {
        return {il, 0, stop, 1};
    }
}

#endif //SLICE_HPP
