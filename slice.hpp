#ifndef SLICE_HPP
#define SLICE_HPP

#include <iterbase.hpp>

#include <iterator>

namespace iter {

    //Forward declarations of Slice and slice
    //template <typename Container, typename DifferenceType>
    //class Slice;

    //template <typename T>
    //Slice<std::initializer_list<T>> slice( std::initializer_list<T> &&);

    //template <typename Container, typename DifferenceType>
    //Slice<Container> slice(Container &&);

    template <typename Container, typename DifferenceType>
    class Slice : public IterBase<Container>{
        private:
            Container & container;
            DifferenceType start;
            DifferenceType stop;
            DifferenceType step;

            // The only thing allowed to directly instantiate an Slice is
            // the slice function
            //friend Slice slice<Container, DifferenceType>(Container &&);
            //template <typename T>
            //friend Slice<std::initializer_list<T>> slice(std::initializer_list<T> &&);

            using typename IterBase<Container>::contained_iter_type;

            using typename IterBase<Container>::contained_iter_ret;

            
        public:
            Slice(Container & container, DifferenceType start,
                  DifferenceType stop, DifferenceType step) :
                container(container),
                start(start),
                stop(stop),
                step(step)
            { 
                if ((start < stop && step <=0) ||
                        (start > stop && step >=0)){
                    this->stop = start;
                } 
            }

            // Value constructor for use only in the slice function
            Slice () = delete;
            Slice & operator=(const Slice &) = delete;

            Slice(const Slice &) = default;


            //  Holds an iterator of the contained type and a size_t for the
            //  index.  Each call to ++ increments both of these data members.
            //  Each dereference returns an IterYield.
            class Iterator {
                private:
                    contained_iter_type sub_iter;
                    DifferenceType step;
                public:
                    Iterator (contained_iter_type si, DifferenceType step) :
                        sub_iter(si),
                        step(step)
                    { }

                    contained_iter_ret operator*() const {
                        return *this->sub_iter;
                    }

                    Iterator & operator++() { 
                        std::advance(this->sub_iter, this->step);
                        return *this;
                    }

                    bool operator!=(const Iterator & other) const {
                        return this->sub_iter != other.sub_iter;
                    }
            };

            Iterator begin() const {
                return Iterator(
                        std::next(std::begin(this->container), this->start),
                        this->step);
            }

            Iterator end() const {
                return Iterator(
                        std::next(std::begin(this->container), this->stop),
                        this->step);
            }

    };
    // Helper function to instantiate an Slice
    template <typename Container, typename DifferenceType>
    Slice<Container, DifferenceType> slice(
            Container && container, DifferenceType start,
            DifferenceType stop, DifferenceType step=1) {
        return Slice<Container, DifferenceType>(
                std::forward<Container>(container), start, stop, step);
    }

    //only give the end as an arg and assume step  is 1 and begin is 0
    template <typename Container, typename DifferenceType>
    Slice<Container, DifferenceType> slice(
            Container && container, DifferenceType stop) {
        return Slice<Container, DifferenceType>(
                std::forward<Container>(container), 0, stop, 1);
    }
}

#endif //SLICE_HPP
