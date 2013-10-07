#ifndef GROUPER_HPP 
#define GROUPER_HPP 

#include "iterator_range.hpp"
#include <vector>
#include <algorithm>
#include <functional>
#include <type_traits>

namespace iter {
    template <typename Container>
    class grouper_iter;

    template <typename Container>
    iterator_range<grouper_iter<Container>> grouper(
            Container & container, size_t s) {
        auto begin = grouper_iter<Container>(container, s);
        auto end = grouper_iter<Container>(container);
        return iterator_range<grouper_iter<Container>>(begin, end);
    }

    template <typename Container>
    class grouper_iter {
        private:
            Container & container;
            using Iterator = decltype(container.begin());
            using Deref_type =
                std::vector<
                    std::reference_wrapper<
                        typename std::remove_reference<
                            decltype(*std::declval<Iterator>())>::type>>;


            std::vector<Iterator> group;

            size_t group_size = 0;
            bool not_done = true;

        public: 
            grouper_iter(Container & c, size_t s) :
                container(c),group_size(s) 
            {
                // if the group size is 0 or the container is empty produce
                // nothing
                if (this->group_size == 0 ||
                        !(this->container.begin() != this->container.end())) {
                    this->not_done = false;
                    return;
                }
                for (size_t i = 0; i < this->group_size; ++i) 
                    this->group.push_back(this->container.begin() + i);
            }

            //seems like conclassor is same as moving_section_iter
            grouper_iter(Container & c) :
                container(c)
            {
                //creates the end iterator
                group.push_back(container.end());
            }

            grouper_iter & operator++() {
                for (auto & iter : this->group) {
                    iter += this->group_size;
                }
                return *this;
            }

            bool operator!=(const grouper_iter &) const {
                return this->not_done;
            }

            Deref_type operator*() {
                Deref_type vec;
                for (auto i : this->group) {
                    if(i == this->container.end()) {
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

#endif // ifndef GROUPER_HPP
