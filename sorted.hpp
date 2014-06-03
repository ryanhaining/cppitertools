#ifndef SORTED__HPP__
#define SORTED__HPP__

#include "iterbase.hpp"

#include <iterator>
#include <algorithm>
#include <vector>

namespace iter {
    template <typename Container, typename CompareFunc>
    class Sorted;

    template <typename Container, typename CompareFunc>
    Sorted<Container, CompareFunc> sorted(Container&&, CompareFunc);

    template <typename Container, typename CompareFunc>
    class Sorted  {
        private:
            Container container;
            std::vector<iterator_type<Container>> sorted_iters;

            friend Sorted
                sorted<Container, CompareFunc>(Container&&, CompareFunc);

            using sorted_iter_type = iterator_type<decltype(sorted_iters)>;


            Sorted() = delete;
            Sorted& operator=(const Sorted&) = delete;

            Sorted(Container in_container, CompareFunc compare_func)
                : container(std::forward<Container>(in_container))
            {
                // Fill the sorted_iters vector with an iterator to each
                // element in the container
                for (auto iter = std::begin(this->container);
                        iter != std::end(this->container);
                        ++iter) {
                    this->sorted_iters.push_back(iter);
                }

                // sort by comparing the elements that the iterators point to
                std::sort(std::begin(sorted_iters), std::end(sorted_iters),
                        [&] (const iterator_type<Container>& it1,
                            const iterator_type<Container>& it2)
                        { return compare_func(*it1, *it2); });
            }

        public:

            Sorted(const Sorted&) = default;

            // Iterates over a series of Iterators, automatically dereferencing
            // them when accessed with operator *
            class IteratorIterator : public sorted_iter_type {
                public:
                    IteratorIterator(sorted_iter_type iter)
                        : sorted_iter_type{iter}
                    { }
                    IteratorIterator(const IteratorIterator&) = default;

                    // Dereference the current iterator before returning
                    iterator_deref<Container> operator*() {
                        return *sorted_iter_type::operator*();
                    }
            };

            IteratorIterator begin() {
                return {std::begin(sorted_iters)};
                
            }

            IteratorIterator end() {
                return {std::end(sorted_iters)};
            }
    };

    template <typename Container, typename CompareFunc>
    Sorted<Container, CompareFunc> sorted(
            Container&& container, CompareFunc compare_func) {
        return {std::forward<Container>(container), compare_func};
    }

    template <typename Container>
    auto sorted(Container&& container) ->
             decltype(sorted(std::forward<Container>(container),
                         std::less<iterator_deref<Container>>()))
     {
         return sorted(std::forward<Container>(container),
                 std::less<iterator_deref<Container>>());
     }

}

#endif //#ifndef SORTED__HPP__
