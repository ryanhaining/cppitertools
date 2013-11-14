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
                // sets stop = start if the range is empty
                if ((start < stop && step <=0) ||
                        (start > stop && step >=0)){
                    this->stop = start;
                } 
                if (this->stop > static_cast<DifferenceType>(container.size())) {
                    this->stop = static_cast<DifferenceType>(container.size());
                }
                if (this->start < 0) {
                    this->start = 0; 
                }
            }

            Slice () = delete;
            Slice & operator=(const Slice &) = delete;

            Slice(const Slice &) = default;


            class Iterator {
                private:
                    contained_iter_type sub_iter;
                    DifferenceType current;
                    const DifferenceType stop;
                    const DifferenceType step;

                public:
                    Iterator (contained_iter_type si, DifferenceType start,
                            DifferenceType stop, DifferenceType step) :
                        sub_iter(si),
                        current(start),
                        stop(stop),
                        step(step)
                    { }

                    contained_iter_ret operator*() const {
                        return *this->sub_iter;
                    }

                    Iterator & operator++() { 
                        std::advance(this->sub_iter, this->step);
                        this->current += this->step;
                        return *this;
                    }

                    bool operator!=(const Iterator &) const {
                        return (this->step > 0 && this->current < this->stop)||
                            (this->step < 0 && this->current > this->stop);
                    }
            };

            Iterator begin() const {
                return Iterator(
                        std::next(std::begin(this->container), this->start),
                        this->start, this->stop, this->step);
            }

            Iterator end() const {
                return Iterator(
                        std::next(std::begin(this->container), this->stop),
                        this->stop, this->stop, this->step);
            }

    };

    // Helper function to instantiate a Slice
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
