#ifndef ITER_SLIDING_WINDOW_HPP_
#define ITER_SLIDING_WINDOW_HPP_

#include "iterbase.hpp"
#include "iteratoriterator.hpp"

#include <deque>
#include <utility>
#include <iterator>

namespace iter {
    template <typename Container>
    class SlidingWindow;

    template <typename Container>
    SlidingWindow<Container> sliding_window(Container&&, std::size_t);

    template <typename T>
    SlidingWindow<std::initializer_list<T>> sliding_window(
            std::initializer_list<T>, std::size_t);

    template <typename Container>
    class SlidingWindow {
        private: Container container;
            std::size_t window_size;

            friend SlidingWindow sliding_window<Container>(
                    Container&&, std::size_t);
            
            template <typename T>
            friend SlidingWindow<std::initializer_list<T>> sliding_window(
                    std::initializer_list<T>, std::size_t);

            SlidingWindow(Container&& container, std::size_t win_sz)
                : container(std::forward<Container>(container)),
                window_size{win_sz}
            { }

            using IndexVector = std::deque<iterator_type<Container>>;
            using DerefVec = IterIterWrapper<IndexVector>;
        public:

            class Iterator
                : public std::iterator<std::input_iterator_tag, DerefVec>
            {
                private:
                    iterator_type<Container> sub_iter;
                    DerefVec window;

                public:
                    Iterator(iterator_type<Container>&& in_iter,
                            const iterator_type<Container>& in_end,
                             std::size_t window_sz)
                        : sub_iter(std::move(in_iter))
                    {
                        std::size_t i{0};
                        while (i < window_sz
                                && this->sub_iter != in_end) {
                            this->window.get().push_back(this->sub_iter);
                            ++i;
                            if (i != window_sz) ++this->sub_iter;
                        }
                    }

                    bool operator!=(const Iterator& other) const {
                        return this->sub_iter != other.sub_iter;
                    }

                    bool operator==(const Iterator& other) const {
                        return !(*this != other);
                    }

                    DerefVec& operator*() {
                        return this->window;
                    }

                    Iterator& operator++() {
                        ++this->sub_iter;
                        this->window.get().pop_front();
                        this->window.get().push_back(this->sub_iter);
                        return *this;
                    }

                    Iterator operator++(int) {
                        auto ret = *this;
                        ++*this;
                        return ret;
                    }
            };

            Iterator begin() {
                return {
                    (this->window_size != 0 ?
                     std::begin(this->container)
                     : std::end(this->container)),
                    std::end(this->container),
                    this->window_size};
            }

            Iterator end() {
                return {std::end(this->container),
                    std::end(this->container),
                    this->window_size};
            }
    };

    template <typename Container>
    SlidingWindow<Container> sliding_window(
            Container&& container, std::size_t window_size) {
        return {std::forward<Container>(container), window_size};
    }

    template <typename T>
    SlidingWindow<std::initializer_list<T>> sliding_window(
            std::initializer_list<T> il, std::size_t window_size) {
        return {std::move(il), window_size};
    }
}

#endif
