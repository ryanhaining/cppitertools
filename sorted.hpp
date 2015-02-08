#ifndef ITER_SORTED_HPP_
#define ITER_SORTED_HPP_

#include "iterbase.hpp"
#include "iteratoriterator.hpp"

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
            using IterIterWrap =
                IterIterWrapper<std::vector<iterator_type<Container>>>;
            using ItIt = iterator_type<IterIterWrap>;
            friend Sorted
                sorted<Container, CompareFunc>(Container&&, CompareFunc);

            Container container;
            IterIterWrap sorted_iters;


            Sorted(Container&& in_container, CompareFunc compare_func)
                : container(std::forward<Container>(in_container))
            {
                // Fill the sorted_iters vector with an iterator to each
                // element in the container
                for (auto iter = std::begin(this->container);
                        iter != std::end(this->container);
                        ++iter) {
                    this->sorted_iters.get().push_back(iter);
                }

                // sort by comparing the elements that the iterators point to
                std::sort(std::begin(sorted_iters.get()),
                        std::end(sorted_iters.get()),
                        [compare_func] (const iterator_type<Container>& it1,
                            const iterator_type<Container>& it2)
                        { return compare_func(*it1, *it2); });
            }

        public:

            ItIt begin() {
                return std::begin(sorted_iters);
                
            }

            ItIt end() {
                return std::end(sorted_iters);
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

#endif
