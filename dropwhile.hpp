#ifndef DROPWHILE__H__
#define DROPWHILE__H__

#include <iterbase.hpp>

#include <utility>
#include <iterator>
#include <initializer_list>

namespace iter {

    //Forward declarations of DropWhile and dropwhile
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
            
            // Value constructor for use only in the dropwhile function
            DropWhile(FilterFunc filter_func, Container container)
                : container(std::forward<Container>(container)),
                filter_func(filter_func)
            { }
            DropWhile() = delete;
            DropWhile& operator=(const DropWhile&) = delete;

        public:
            DropWhile(const DropWhile&) = default;
            class Iterator {
                private:
                    iterator_type<Container> sub_iter;
                    const iterator_type<Container> sub_end;
                    FilterFunc filter_func;

                    // skip all values for which the predicate is true
                    void skip_passes() { 
                        while (this->sub_iter != this->sub_end
                                && this->filter_func(*this->sub_iter)) {
                            ++this->sub_iter;
                        }
                    }

                public:
                    Iterator (iterator_type<Container> iter,
                            iterator_type<Container> end,
                            FilterFunc filter_func)
                        : sub_iter{iter},
                        sub_end{end},
                        filter_func(filter_func)
                    { 
                        this->skip_passes();
                    } 

                    iterator_deref<Container> operator*() const {
                        return *this->sub_iter;
                    }

                    Iterator& operator++() { 
                        ++this->sub_iter;
                        return *this;
                    }

                    bool operator!=(const Iterator& other) const {
                        return this->sub_iter != other.sub_iter;
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

    // Helper function to instantiate a DropWhile
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

#endif //ifndef DROPWHILE__H__
