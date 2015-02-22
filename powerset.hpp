#ifndef ITER_POWERSET_HPP_
#define ITER_POWERSET_HPP_

#include "iterbase.hpp"
#include "combinations.hpp"

#include <cassert>
#include <memory>
#include <initializer_list>
#include <utility>
#include <iterator>
#include <type_traits>

namespace iter {
    template <typename Container>
    class Powersetter {
        private:
            Container container;
            using CombinatorType =
                decltype(combinations(std::declval<Container&>(), 0));

        public:
            Powersetter(Container&& in_container)
                : container(std::forward<Container>(in_container))
            { }

            class Iterator
                : public std::iterator<
                      std::input_iterator_tag, CombinatorType>
            {
                private:
                    typename std::remove_reference<Container>::type *container_p;
                    std::size_t set_size;
                    std::shared_ptr<CombinatorType> comb;
                    iterator_type<CombinatorType> comb_iter;
                    iterator_type<CombinatorType> comb_end;

                public:
                    Iterator(Container& in_container, std::size_t sz)
                        : container_p{&in_container},
                        set_size{sz},
                        comb{new CombinatorType(combinations(in_container, sz))},
                        comb_iter{std::begin(*comb)},
                        comb_end{std::end(*comb)}
                    { }

                    Iterator& operator++() {
                        ++this->comb_iter;
                        if (this->comb_iter == this->comb_end) {
                            ++this->set_size;
                            this->comb.reset(new CombinatorType(combinations(
                                            *this->container_p, this->set_size)));
                            this->comb_iter = std::begin(*this->comb);
                            this->comb_end = std::end(*this->comb);
                        }
                        return *this;
                    }

                    Iterator operator++(int) {
                        auto ret = *this;
                        ++*this;
                        return ret;
                    }

                    iterator_deref<CombinatorType> operator*() {
                        return *this->comb_iter;
                    }

                    bool operator != (const Iterator& other) const {
                        return !(*this == other);
                    }

                    bool operator==(const Iterator& other) const {
                        return this->set_size == other.set_size
                            && this->comb_iter == other.comb_iter;
                    }
            };

            Iterator begin() {
                return {this->container, 0};
            }

            Iterator end() {
                return {this->container, dumb_size(this->container) + 1};
            }
    };

    template <typename Container>
    Powersetter<Container> powerset(Container&& container) {
        return {std::forward<Container>(container)};
    }

    template <typename T>
    Powersetter<std::initializer_list<T>> powerset(
            std::initializer_list<T> il) {
        return {std::move(il)};
    }
}
#endif
