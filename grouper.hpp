#ifndef GROUPER_HPP 
#define GROUPER_HPP 

#include "iterator_range.hpp"
#include <vector>
#include <algorithm>
#include <functional>
#include <type_traits>

namespace iter {
    template <typename Container>
        struct grouper_iter;
    template <typename Container>
        iterator_range<grouper_iter<Container>>
        grouper(Container & container, size_t s) {
            auto begin = grouper_iter<Container>(container,s);
            auto end = grouper_iter<Container>(container);
            return iterator_range<grouper_iter<Container>>(begin,end);
        }

    template <typename Container>
        struct grouper_iter {
            Container & container;
            using Iterator = decltype(container.begin());
            std::vector<Iterator> group;
            size_t group_size = 0;
            bool not_done = true;
            grouper_iter(Container & c, size_t s) :
                container(c),group_size(s) 
            {
                for (size_t i = 0; i < group_size; ++i) 
                    group.push_back(container.begin()+i);
            }
            //seems like constructor is same as moving_section_iter
            grouper_iter(Container & c) : container(c)
            //creates the end iterator
            {
                group.push_back(container.end());
            }

            grouper_iter & operator++() {
                for (auto & iter : group) {
                    iter += group_size;
                }
                return *this;
            }
            bool operator!=(const grouper_iter &) {
                return not_done;
            }
            using Deref_type = std::vector<std::reference_wrapper<typename std::remove_reference<decltype(*std::declval<Iterator>())>::type>>;
            Deref_type operator*()
            {
                Deref_type vec;
                for (auto i : group) {
                    if(i == container.end()) {
                        not_done = false;
                        break;
                    } 
                    //if the group is at the end the vector will be smaller 
                    else { 
                        vec.push_back(*i);
                    }
                }
                return vec;
            }
        };
}

#endif //MOVING_SECTION_HPP
