#ifndef SLIDING_WINDOW_HPP_
#define SLIDING_WINDOW_HPP_

#include "iterator_range.hpp"
#include "iterbase.hpp"

#include <vector>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <utility>
#include <iterator>

namespace iter {
    template <typename Container>
    class SlidingWindow {
        private:
            Container container;
            std::size_t window_size;

        public:
            SlidingWindow(Container container, std::size_t win_sz)
                : container(std::forward<Container>(container)),
                window_size{win_sz}
            { }

            class Iterator {
                private:
                    // confusing, but, just makes the type of the vector returned by
                    // operator*()
                    using OpDerefElemType =
                        std::reference_wrapper<
                            typename std::remove_reference<
                                iterator_deref<Container>>::type>;
                    using DerefVec = std::vector<OpDerefElemType>;

                    std::vector<iterator_type<Container>> section;
                    std::size_t section_size = 0;

                public:
                    Iterator(Container& container, std::size_t s)
                        : section_size{s}
                    {
                        auto iter = std::begin(container);
                        auto end = std::end(container);
                        for (std::size_t i = 0;
                                i < section_size && iter != end;
                                ++iter, ++i) {
                            section.push_back(iter);
                        }
                    }

                    // for the end iter
                    Iterator(Container& container)
                        : section{std::end(container)},
                        section_size{0}
                    { }

                    Iterator& operator++() {
                        for (auto&& iter : section) {
                            ++iter;
                        }
                        return *this;
                    }

                    bool operator!=(const Iterator& rhs) const {
                       return this->section.back() != rhs.section.back();
                    }

                    DerefVec operator*() {
                        DerefVec vec;
                        for (auto&& iter : section) {
                            vec.push_back(*iter);
                        }
                        return vec;
                    }
            };

            Iterator begin() {
                return {container, window_size};
            }

            Iterator end() {
                return {container};
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
        return {il, window_size};
    }
}

#endif //SLIDING_WINDOW_HPP_
