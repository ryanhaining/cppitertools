#ifndef GROUPER_HPP 
#define GROUPER_HPP 

#include "iterator_range.hpp"

#include <vector>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <utility>
#include <iterator>

namespace iter {
    template <typename Container>
    class grouper_iter;

    template <typename Container>
    iterator_range<grouper_iter<Container>> grouper(
            Container && container, size_t s) {
        auto begin = grouper_iter<Container>(std::forward<Container>(container), s);
        auto end = grouper_iter<Container>(std::forward<Container>(container));
        return iterator_range<grouper_iter<Container>>(begin, end);
    }

    template <typename Container>
    class grouper_iter {
        private:
            typename 
                std::conditional<std::is_lvalue_reference<Container>::value,
                Container&,
                const Container &>::type container;
            //Container && container;
            //using Iterator = decltype(container.begin());
            using Iterator = decltype(std::begin(container));
            using Deref_type =
                std::vector<
                    std::reference_wrapper<
                        typename std::remove_reference<
                            decltype(*std::declval<Iterator>())>::type>>;


            std::vector<Iterator> group;

            size_t group_size = 0;
            bool not_done = true;

        public: 
            grouper_iter(Container && c, size_t s) :
                container(std::forward<Container>(c)),group_size(s) 
            {
                // if the group size is 0 or the container is empty produce
                // nothing
                if (this->group_size == 0 ||
                        !(std::begin(this->container) != std::end(this->container))) {
                    this->not_done = false;
                    return;
                }
                size_t i = 0;
                for (auto iter = std::begin(container); i < group_size;++i,++iter) {
                    group.push_back(iter);
                }
                //for (size_t i = 0; i < this->group_size; ++i) 
                  //  this->group.push_back(this->container.begin() + i);
            }

            //seems like conclassor is same as moving_section_iter
            grouper_iter(Container && c) :
                container(std::forward<Container>(c))
            {
                //creates the end iterator
                group.push_back(std::end(container));
            }

            //plan to conditionally check for existence of +=
            grouper_iter & operator++() {
                for (auto & iter : this->group) {
                    std::advance(iter,this->group_size);
                }
                return *this;
            }
            /*
            grouper_iter & operator++() {
                for (auto & iter : this->group) {
                    for(size_t i = 0; i < group_size;++i,++iter);
                }
                return *this;
            }
            */
            bool operator!=(const grouper_iter &) const {
                return this->not_done;
            }

            Deref_type operator*() {
                Deref_type vec;
                for (auto i : this->group) {
                    if(!(i != std::end(this->container))) {
                        this->not_done = false;
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
namespace std {
    template <typename Container>
        struct iterator_traits<iter::grouper_iter<Container>> {
            using difference_type = ptrdiff_t;
            using iterator_category = input_iterator_tag;
        };
}
#endif // ifndef GROUPER_HPP
