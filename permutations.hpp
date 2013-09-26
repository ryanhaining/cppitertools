#ifndef PERMUTATIONS_HPP
#define PERMUTATIONS_HPP

#include <iterator_range.hpp>
#include <algorithm>

namespace iter {
    template <typename Container>
        struct permutation_iter;

    template <typename Container>
        iterator_range<permutation_iter<Container>>
        permutations (const Container & container) {
            return iterator_range<permutation_iter<Container>>(
                    permutation_iter<Container>(container),
                    permutation_iter<Container>());
        }
    template <typename Container>
        struct permutation_iter
        {
            Container container;
            using Iterator = decltype(container.begin());
            bool is_not_last = true;
            permutation_iter(){}
            permutation_iter(const Container & c) : container(c) 
            {
                //sort first so you can get every permutation
                std::sort(container.begin(),container.end());
            }
            const Container & operator*()
            {
                return container;
            }
            permutation_iter & operator++() {
                is_not_last = std::next_permutation(container.begin(),container.end());
                return *this;
            }
            bool operator!=(const permutation_iter &) {
                return is_not_last;
            }
        };
}

#endif //PERMUTATIONS_HPP

