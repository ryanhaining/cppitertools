#ifndef ITER_COMPRESS_H_
#define ITER_COMPRESS_H_

#include "iterbase.hpp"

#include <utility>
#include <iterator>
#include <initializer_list>

namespace iter {

    //Forward declarations of Compressed and compress
    template <typename Container, typename Selector>
    class Compressed;

    template <typename Container, typename Selector>
    Compressed<Container, Selector> compress(Container&&, Selector&&);

    template <typename T, typename Selector>
    Compressed<std::initializer_list<T>, Selector> compress(
            std::initializer_list<T>, Selector&&);

    template <typename Container, typename T>
    Compressed<Container, std::initializer_list<T>> compress(
            Container&&, std::initializer_list<T>);

    template <typename T, typename U>
    Compressed<std::initializer_list<T>, std::initializer_list<U>> compress(
            std::initializer_list<T>, std::initializer_list<U>);

    template <typename Container, typename Selector>
    class Compressed  {
        private:
            Container container;
            Selector selectors;

            // The only thing allowed to directly instantiate an Compressed is
            // the compress function
            friend Compressed compress<Container, Selector>(
                    Container&&, Selector&&);

            template <typename T, typename Sel>
            friend Compressed<std::initializer_list<T>, Sel> compress(
                    std::initializer_list<T>, Sel&&);

            template <typename Con, typename T>
            friend Compressed<Con, std::initializer_list<T>> compress(
                    Con&&, std::initializer_list<T>);

            template <typename T, typename U>
            friend Compressed<std::initializer_list<T>,
                   std::initializer_list<U>> compress(
                    std::initializer_list<T>, std::initializer_list<U>);

            
            // Selector::Iterator type
            using selector_iter_type = decltype(std::begin(selectors));
            
            // Value constructor for use only in the compress function
            Compressed(Container&& container, Selector&& selectors)
                : container(std::forward<Container>(container)),
                selectors(std::forward<Selector>(selectors))
            { }

        public:

            class Iterator 
                : public std::iterator<std::input_iterator_tag,
                        iterator_traits_deref<Container>>
            {
                private:
                    iterator_type<Container> sub_iter;
                    iterator_type<Container> sub_end;

                    selector_iter_type selector_iter;
                    selector_iter_type selector_end;

                    void increment_iterators() {
                        ++this->sub_iter;
                        ++this->selector_iter;
                    }

                    void skip_failures() {
                        while (this->sub_iter != this->sub_end
                               && this->selector_iter != this->selector_end
                                && !*this->selector_iter) {
                            this->increment_iterators();
                        }
                    }

                public:
                    Iterator(iterator_type<Container>&& cont_iter,
                            iterator_type<Container>&& cont_end,
                            selector_iter_type&& sel_iter,
                            selector_iter_type&& sel_end)
                        : sub_iter{std::move(cont_iter)},
                        sub_end{std::move(cont_end)},
                        selector_iter{std::move(sel_iter)},
                        selector_end{std::move(sel_end)}
                    { 
                        this->skip_failures();
                    } 

                    iterator_deref<Container> operator*() {
                        return *this->sub_iter;
                    }

                    Iterator& operator++() { 
                        this->increment_iterators();
                        this->skip_failures();
                        return *this;
                    }

                    Iterator operator++(int) {
                        auto ret = *this;
                        ++*this;
                        return ret;
                    }

                    bool operator!=(const Iterator& other) const {
                        return this->sub_iter != other.sub_iter
                            && this->selector_iter != other.selector_iter;
                    }

                    bool operator==(const Iterator& other) const {
                        return !(*this != other);
                    }
            };

            Iterator begin() {
                return {std::begin(this->container), std::end(this->container),
                    std::begin(this->selectors), std::end(this->selectors)};
            }

            Iterator end() {
                return {std::end(this->container), std::end(this->container),
                        std::end(this->selectors), std::end(this->selectors)};
            }

    };

    // Helper function to instantiate an Compressed
    template <typename Container, typename Selector>
    Compressed<Container, Selector> compress(
            Container&& container, Selector&& selectors) {
        return {std::forward<Container>(container),
                std::forward<Selector>(selectors)};
    }

    template <typename T, typename Selector>
    Compressed<std::initializer_list<T>, Selector> compress(
            std::initializer_list<T> data, Selector&& selectors) {
        return {std::move(data),
                std::forward<Selector>(selectors)};
    }

    template <typename Container, typename T>
    Compressed<Container, std::initializer_list<T>> compress(
            Container&& container, std::initializer_list<T> selectors) {
        return {std::forward<Container>(container),
                std::move(selectors)};
    }

    template <typename T, typename U>
    Compressed<std::initializer_list<T>, std::initializer_list<U>> compress(
            std::initializer_list<T> data,
            std::initializer_list<U> selectors) {
        return {std::move(data),
                std::move(selectors)};
    }
}

#endif
