#ifndef MOVING_SECTION_HPP
#define MOVING_SECTION_HPP

#include "iterator_range.hpp"

#include <vector>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <utility>
#include <iterator>

namespace iter {
    template <typename Container>
        struct moving_section_iter;
    template <typename Container>
        iterator_range<moving_section_iter<Container>>
        moving_section(Container && container, size_t s) {
            auto begin = moving_section_iter<Container>(std::forward<Container>(container),s);
            auto end = moving_section_iter<Container>(std::forward<Container>(container));
            return iterator_range<moving_section_iter<Container>>(begin,end);
        }

    template <typename Container>
        struct moving_section_iter {
            typename 
                std::conditional<std::is_lvalue_reference<Container>::value,
                Container&,
                const Container &>::type container;
            //Container && container;
            //using Iterator = decltype(container.begin());
            using Iterator = decltype(std::begin(container));
            std::vector<Iterator> section;
            size_t section_size = 0;
            moving_section_iter(Container && c, size_t s) :
                container(std::forward<Container>(c)),section_size(s) {
                    size_t i = 0;
                    for (auto iter = std::begin(container); i < section_size;++iter,++i) {
                        section.push_back(iter);
                    }
                    //for (size_t i = 0; i < section_size; ++i) 
                      //  section.push_back(container.begin()+i);
                }
            moving_section_iter(Container && c) : container(std::forward<Container>(c))
            //creates the end iterator
            {
                section.push_back(std::end(container));
            }

            moving_section_iter & operator++() {
                for (auto & iter : section) {
                    ++iter;
                }
                return *this;
                //std::for_each(section.begin(),section.end(),[](Iterator & i){++i;});
            }
            bool operator!=(const moving_section_iter & rhs) {
               return this->section.back() != rhs.section.back();
            }
            using Deref_type = std::vector<std::reference_wrapper<typename std::remove_reference<decltype(*std::declval<Iterator>())>::type>>;
            Deref_type operator*()
            {
                Deref_type vec;
                for (auto i : section) {
                    vec.push_back(*i);
                }
                return vec;
            }
        };
}
namespace std {
    template <typename Container>
        struct iterator_traits<iter::moving_section_iter<Container>> {
            using difference_type = ptrdiff_t;
            using iterator_category = input_iterator_tag;
        };
}
#endif //MOVING_SECTION_HPP
