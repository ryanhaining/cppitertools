#ifndef ITER_DROPWHILE_H_
#define ITER_DROPWHILE_H_

#include "iterbase.hpp"

#include <utility>
#include <iterator>
#include <initializer_list>

namespace iter {

    template <typename FilterFunc, typename Container>
    class DropWhile;

    template <typename FilterFunc, typename Container>
    DropWhile<FilterFunc, Container> dropwhile(FilterFunc, Container&&);

    template <typename FilterFunc, typename T>
    DropWhile<FilterFunc, std::initializer_list<T>> dropwhile(
            FilterFunc, std::initializer_list<T>);

    template <typename FilterFunc, typename Container>
    class DropWhile  {
        private:
            Container container;
            FilterFunc filter_func;

            friend DropWhile dropwhile<FilterFunc, Container>(
                    FilterFunc, Container&&);

            template <typename FF, typename T>
            friend DropWhile<FF, std::initializer_list<T>> dropwhile(
                    FF, std::initializer_list<T>);
            
            DropWhile(FilterFunc filter_func, Container&& container)
                : container(std::forward<Container>(container)),
                filter_func(filter_func)
            { }

        public:
            class Iterator 
                : public std::iterator<std::input_iterator_tag,
                    iterator_traits_deref<Container>>
            {
                private:
                    iterator_type<Container> sub_iter;
                    iterator_type<Container> sub_end;
                    DerefHolder<iterator_deref<Container>> item;
                    FilterFunc *filter_func;

                    void inc_sub_iter() {
                            ++this->sub_iter;
                            if (this->sub_iter != this->sub_end) {
                                this->item.reset(*this->sub_iter);
                            }
                    }

                    // skip all values for which the predicate is true
                    void skip_passes() { 
                        while (this->sub_iter != this->sub_end
                                && (*this->filter_func)(this->item.get())) {
                            this->inc_sub_iter();
                        }
                    }

                public:
                    Iterator(iterator_type<Container>&& iter,
                            iterator_type<Container>&& end,
                            FilterFunc& filter_func)
                        : sub_iter{std::move(iter)},
                        sub_end{std::move(end)},
                        filter_func(&filter_func)
                    { 
                        if (this->sub_iter != this->sub_end) {
                            this->item.reset(*this->sub_iter);
                        }
                        this->skip_passes();
                    } 

                    iterator_deref<Container> operator*() {
                        return this->item.pull();
                    }

                    Iterator& operator++() { 
                        this->inc_sub_iter();
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
                        this->filter_func};
            }

            Iterator end() {
                return {std::end(this->container),
                        std::end(this->container),
                        this->filter_func};
            }

    };

    template <typename FilterFunc, typename Container>
    DropWhile<FilterFunc, Container> dropwhile(
            FilterFunc filter_func, Container&& container) {
        return {filter_func, std::forward<Container>(container)};
    }

    template <typename FilterFunc, typename T>
    DropWhile<FilterFunc, std::initializer_list<T>> dropwhile(
            FilterFunc filter_func, std::initializer_list<T> il)
    {
        return {filter_func, std::move(il)};
    }
}

#endif
