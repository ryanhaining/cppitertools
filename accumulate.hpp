#ifndef ACCUMULATE__H__
#define ACCUMULATE__H__

#include "iterbase.hpp"

#include <utility>
#include <iterator>
#include <initializer_list>
#include <functional>

namespace iter {

    //Forward declarations of Accumulator and accumulate
    template <typename Container, typename AccumulateFunc>
    class Accumulator;

    template <typename Container, typename AccumulateFunc>
    Accumulator<Container, AccumulateFunc> accumulate(
            Container&&, AccumulateFunc);

    template <typename T, typename AccumulateFunc>
    Accumulator<std::initializer_list<T>, AccumulateFunc> accumulate(
            std::initializer_list<T>&&, AccumulateFunc);

    template <typename Container, typename AccumulateFunc>
    class Accumulator {
        private:
            Container& container;
            AccumulateFunc accumulate_func;

            // The accumulate function is the only thing allowed to create a Accumulator
            friend Accumulator accumulate<Container, AccumulateFunc>(
                    Container&&, AccumulateFunc);

            template <typename T, typename AF>
            friend Accumulator<std::initializer_list<T>, AF> accumulate(
                    std::initializer_list<T>&&, AF);
            
            // Value constructor for use only in the accumulate function
            Accumulator(Container&& container, AccumulateFunc accumulate_func)
                : container{container},
                accumulate_func(accumulate_func)
            { }
            Accumulator () = delete;
            Accumulator & operator=(const Accumulator&) = delete;

        public:
            Accumulator(const Accumulator&) = default;

            class Iterator {
                // AccumVal must be default constructible
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
                        // only get first value if not an end iterator
                        acc_val(!(iter != end) ? AccumVal{} : *iter)
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
    Accumulator<Container, AccumulateFunc> accumulate(
            Container&& container,
            AccumulateFunc accumulate_func)
    {
        return {std::forward<Container>(container), accumulate_func};
    }

    template <typename Container>
    auto accumulate(Container&& container) -> 
        decltype(accumulate(std::forward<Container>(container),
                    std::plus<iterator_deref<Container>>{}))
    {
        return accumulate(std::forward<Container>(container),
                std::plus<iterator_deref<Container>>{});
    }

    template <typename T, typename AccumulateFunc>
    Accumulator<std::initializer_list<T>, AccumulateFunc> accumulate(
            std::initializer_list<T>&& il,
            AccumulateFunc accumulate_func)
    {
        return {std::move(il), accumulate_func};
    }

    template <typename T>
    auto accumulate(std::initializer_list<T>&& il) ->
        decltype(accumulate(std::move(il), std::plus<T>{}))
    {
        return accumulate(std::move(il), std::plus<T>{});
    }

}

#endif //ifndef ACCUMULATE__H__
