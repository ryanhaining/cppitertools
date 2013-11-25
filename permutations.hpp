#ifndef PERMUTATIONS_HPP
#define PERMUTATIONS_HPP

#include "iterator_range.hpp"

#include <algorithm>
#include <initializer_list>
#include <vector>

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
    //since initializer_list doesn't have bidir iters this is a hack
    //to get it to work by using a vector in its place
    template <typename T>
        iterator_range<permutation_iter<std::vector<T>>>
        permutations (std::initializer_list<T> && container) {
            std::vector<T> begin(std::begin(container),std::end(container));
            return  {permutation_iter<std::vector<T>>(container),
                    permutation_iter<std::vector<T>>()};
        }

    template <typename Container>
        struct permutation_iter
        {
            Container container;
            using Iterator = decltype(std::begin(container));
            bool is_not_last = true;
            permutation_iter(){}
            permutation_iter(const Container & c) : container(c) 
            {
                //sort first so you can get every permutation
                std::sort(std::begin(container),std::end(container));
            }
            const Container & operator*()
            {
                return container;
            }
            permutation_iter & operator++() {
                is_not_last = std::next_permutation(std::begin(container),std::end(container));
                return *this;
            }
            bool operator!=(const permutation_iter &) {
                return is_not_last;
            }
        };
}
namespace std {
    template <typename Container>
        struct iterator_traits<iter::permutation_iter<Container>> {
            using difference_type = ptrdiff_t;
            using iterator_category = input_iterator_tag;
        };
}
#endif //PERMUTATIONS_HPP

