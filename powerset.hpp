#ifndef POWERSET_HPP
#define POWERSET_HPP

#include "iterator_range.hpp"
#include "combinations.hpp"

#include <initializer_list>

namespace iter {
    template <typename Container>
        struct powerset_iter;

    template <typename Container>
        iterator_range<powerset_iter<Container>>
        powerset(const Container & container)
        {
            auto begin = powerset_iter<Container>(container);
            auto end = powerset_iter<Container>(container);
            return iterator_range<powerset_iter<Container>>(begin,end);
        }

    template <typename T>
        iterator_range<powerset_iter<std::initializer_list<T>>>
        powerset(std::initializer_list<T> && container)
        {
            auto begin = powerset_iter<std::initializer_list<T>>(container);
            auto end = powerset_iter<std::initializer_list<T>>(container);
            return {begin,end};
        }


    template <typename Container>
        struct powerset_iter {
        private:
            const Container & container;
            size_t list_size = 0;
            std::vector<combinations_iter<Container>> inner_iters;
            bool not_done = true;
        public:
            powerset_iter(const Container & c) : 
                container(c),
                inner_iters()
                {
                    for (size_t i = 0; i <= container.size();++i) {
                        inner_iters.push_back(combinations_iter<Container>(container,i));
                    }
                }
            //default constructor
            powerset_iter & operator++()
            {
                ++(inner_iters[list_size]);
                
                if (!(inner_iters[list_size] != inner_iters[list_size])) {
                    ++list_size;
                    //inner_iter = combinations_iter<Container>(container,list_size);
                }
                if (container.size() < list_size) not_done = false;
                return *this;
            }
            auto operator*() const -> decltype(*inner_iters[list_size]) 
            {
                return *(inner_iters[list_size]);
            }
            bool operator != (const powerset_iter &) {
               return not_done;
            }
        }; 
}
namespace std {
    template <typename Container>
        struct iterator_traits<iter::powerset_iter<Container>> {
            using difference_type = ptrdiff_t;
            using iterator_category = input_iterator_tag;
        };
}
#endif //POWERSET_HPP
