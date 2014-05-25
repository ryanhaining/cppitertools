#ifndef accumulate__H__
#define accumulate__H__

#include "iterbase.hpp"

#include <utility>
#include <iterator>
#include <initializer_list>

namespace iter {

    //Forward declarations of Accumulator and accumulate
    template <typename AccumulateFunc, typename Container>
    class Accumulator;

    template <typename Container, typename AccumulateFunc>
    Accumulator<AccumulateFunc, Container> accumulate(
            Container &&, AccumulateFunc);

    template <typename AccumulateFunc, typename T>
    Accumulator<AccumulateFunc, std::initializer_list<T>> accumulate(
            std::initializer_list<T> &&, AccumulateFunc);

    template <typename AccumulateFunc, typename Container>
    class Accumulator {
        private:
            Container & container;
            AccumulateFunc accumulate_func;

            // The accumulate function is the only thing allowed to create a Accumulator
            friend Accumulator accumulate<Container, AccumulateFunc>(
                    Container &&, AccumulateFunc);

            template <typename AF, typename T>
            friend Accumulator<std::initializer_list<T>, AF> accumulate(
                    std::initializer_list<T> &&, AF);
            
            // Value constructor for use only in the accumulate function
            Accumulator(AccumulateFunc accumulate_func, Container && container)
                : container{container},
                accumulate_func(accumulate_func)
            { }
            Accumulator () = delete;
            Accumulator & operator=(const Accumulator &) = delete;

        public:
            Accumulator(const Accumulator &) = default;

            class Iterator {
                // NOTE can AccumVal and iterator_deref<> be different?
                using AccumVal =
                    typename std::result_of<AccumulateFunc(
                            iterator_deref<Container>,
                            iterator_deref<Container>)>::type;
                private:
                    iterator_type<Container> sub_iter;
                    const iterator_type<Container> sub_end;
                    AccumulateFunc accumulate_func;
                    AccumVal acc_val;
                public:
                    Iterator (iterator_type<Container> iter,
                            iterator_type<Container> end,
                            AccumulateFunc accumulate_func)
                        : sub_iter{iter},
                        sub_end{end},
                        accumulate_func(accumulate_func),
                        acc_val(iter == end ? AccumVal{} : *iter)
                    { } 

                    AccumVal operator*() const {
                        return this->acc_val;
                    }

                    Iterator & operator++() { 
                        ++this->sub_iter;
                        if (this->sub_iter != this->sub_end) {
                            this->acc_val = accumulate_func(
                                    this->acc_val, *this->sub_iter);
                        }
                        return *this;
                    }

                    bool operator!=(const Iterator & other) const {
                        return this->sub_iter != other.sub_iter;
                    }
            };

            Iterator begin() const {
                return {std::begin(this->container),
                        std::end(this->container),
                        this->accumulate_func};
            }

            Iterator end() const {
                return {std::end(this->container),
                        std::end(this->container),
                        this->accumulate_func};
            }

    };

    // Helper function to instantiate an Accumulator
    template <typename Container, typename AccumulateFunc>
    Accumulator<AccumulateFunc, Container> accumulate(
            Container && container,
            AccumulateFunc accumulate_func)
    {
        return {accumulate_func, std::forward<Container>(container)};
    }

    template <typename T, typename AccumulateFunc>
    Accumulator<AccumulateFunc, std::initializer_list<T>> accumulate(
            std::initializer_list<T> && il,
            AccumulateFunc accumulate_func)
    {
        return {accumulate_func, std::move(il)};
    }

}

#endif //ifndef accumulate__H__
