#ifndef ITER_ACCUMULATE_H_
#define ITER_ACCUMULATE_H_

#include "iterbase.hpp"

#include <utility>
#include <iterator>
#include <initializer_list>
#include <functional>
#include <type_traits>

namespace iter {

    //Forward declarations of Accumulator and accumulate
    template <typename Container, typename AccumulateFunc>
    class Accumulator;

    template <typename Container, typename AccumulateFunc>
    Accumulator<Container, AccumulateFunc> accumulate(
            Container&&, AccumulateFunc);

    template <typename T, typename AccumulateFunc>
    Accumulator<std::initializer_list<T>, AccumulateFunc> accumulate(
            std::initializer_list<T>, AccumulateFunc);

    template <typename Container, typename AccumulateFunc>
    class Accumulator {
        private:
            Container container;
            AccumulateFunc accumulate_func;

            friend Accumulator accumulate<Container, AccumulateFunc>(
                    Container&&, AccumulateFunc);

            template <typename T, typename AF>
            friend Accumulator<std::initializer_list<T>, AF> accumulate(
                    std::initializer_list<T>, AF);

            // AccumVal must be default constructible
            using AccumVal =
                typename std::remove_reference<
                    typename std::result_of<AccumulateFunc(
                            iterator_deref<Container>,
                            iterator_deref<Container>)>::type>::type;
            static_assert(
                    std::is_default_constructible<AccumVal>::value,
                    "Cannot accumulate a non-default constructible type");

            Accumulator(Container&& container, AccumulateFunc accumulate_func)
                : container(std::forward<Container>(container)),
                accumulate_func(accumulate_func)
            { }
        public:

            class Iterator 
                : public std::iterator<std::input_iterator_tag, AccumVal>
            {
                private:
                    iterator_type<Container> sub_iter;
                    iterator_type<Container> sub_end;
                    AccumulateFunc accumulate_func;
                    AccumVal acc_val;
                public:
                    Iterator (iterator_type<Container>&& iter,
                            iterator_type<Container>&& end,
                            AccumulateFunc accumulate_func)
                        : sub_iter{std::move(iter)},
                        sub_end{std::move(end)},
                        accumulate_func(accumulate_func),
                        // only get first value if not an end iterator
                        acc_val(!(iter != end) ? AccumVal{} : *iter)
                    { } 

                    const AccumVal& operator*() const {
                        return this->acc_val;
                    }

                    Iterator& operator++() { 
                        ++this->sub_iter;
                        if (this->sub_iter != this->sub_end) {
                            this->acc_val = accumulate_func(
                                    this->acc_val, *this->sub_iter);
                        }
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
                return {std::begin(this->container),
                        std::end(this->container),
                        this->accumulate_func};
            }

            Iterator end() {
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
                    std::plus<typename std::remove_reference<
                        iterator_deref<Container>>::type>{}))
    {
        return accumulate(std::forward<Container>(container),
                    std::plus<typename std::remove_reference<
                        iterator_deref<Container>>::type>{});
    }

    template <typename T, typename AccumulateFunc>
    Accumulator<std::initializer_list<T>, AccumulateFunc> accumulate(
            std::initializer_list<T> il,
            AccumulateFunc accumulate_func)
    {
        return {std::move(il), accumulate_func};
    }

    template <typename T>
    auto accumulate(std::initializer_list<T> il) ->
        decltype(accumulate(std::move(il), std::plus<T>{}))
    {
        return accumulate(std::move(il), std::plus<T>{});
    }

}

#endif
