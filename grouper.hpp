#ifndef ITER_GROUPER_HPP_
#define ITER_GROUPER_HPP_

#include "iterbase.hpp"
#include "iteratoriterator.hpp"

#include <vector>
#include <algorithm>
#include <type_traits>
#include <functional>
#include <utility>
#include <iterator>
#include <initializer_list>

namespace iter {
    template <typename Container>
    class Grouper;

    template <typename Container>
    Grouper<Container> grouper(Container&&, std::size_t);

    template <typename T>
    Grouper<std::initializer_list<T>> grouper(
            std::initializer_list<T>, std::size_t);

    template <typename Container>
    class Grouper {
        private:
            Container container;
            std::size_t group_size;

            Grouper(Container&& c, std::size_t sz)
                : container(std::forward<Container>(c)),
                group_size{sz}
            { }

            friend Grouper grouper<Container>(Container&&, std::size_t);
            template <typename T>
            friend Grouper<std::initializer_list<T>> grouper(
                    std::initializer_list<T>, std::size_t);

            using IndexVector = std::vector<iterator_type<Container>>;
            using DerefVec = IterIterWrapper<IndexVector>;
        public:
            class Iterator :
                public std::iterator<std::input_iterator_tag, DerefVec>
            {
                private:
                    iterator_type<Container> sub_iter;
                    iterator_type<Container> sub_end;
                    DerefVec group;
                    std::size_t group_size = 0;

                    bool done() const {
                        return this->group.empty();
                    }

                    void refill_group() {
                        this->group.get().clear();
                        std::size_t i{0};
                        while (i < group_size
                                    && this->sub_iter != this->sub_end) {
                            group.get().push_back(this->sub_iter);
                            ++this->sub_iter;
                            ++i;
                        }
                    }

                public: 
                    Iterator(iterator_type<Container>&& in_iter,
                            iterator_type<Container>&& in_end,
                            std::size_t s)
                        : sub_iter{std::move(in_iter)},
                        sub_end{std::move(in_end)},
                        group_size{s} 
                    {
                        this->group.get().reserve(this->group_size);
                        this->refill_group();
                    }

                    Iterator& operator++() {
                        this->refill_group();
                        return *this;
                    }

                    Iterator operator++(int) {
                        auto ret = *this;
                        ++*this;
                        return ret;
                    }

                    bool operator!=(const Iterator& other) const {
                        return !(*this == other);
                    }

                    bool operator==(const Iterator& other) const {
                        return this->done() == other.done()
                            && (this->done()
                                    || !(this->sub_iter != other.sub_iter));
                    }


                    DerefVec& operator*() {
                        return this->group;
                    }
            };

            Iterator begin() {
                return {std::begin(this->container),
                    std::end(this->container),
                    group_size};
            }

            Iterator end() {
                return {std::end(this->container),
                    std::end(this->container),
                    group_size};
            }
    };

    template <typename Container>
    Grouper<Container> grouper(Container&& container, std::size_t group_size) {
        return {std::forward<Container>(container), group_size};
    }

    template <typename T>
    Grouper<std::initializer_list<T>> grouper(
            std::initializer_list<T> il, std::size_t group_size) {
        return {std::move(il), group_size};
    }
}
#endif
