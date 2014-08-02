#ifndef COMBINATIONS_WITH_REPLACEMENT_HPP
#define COMBINATIONS_WITH_REPLACEMENT_HPP

#include "iterator_range.hpp"
#include "iterbase.hpp"

#include <vector>
#include <type_traits>

namespace iter {
    //if size isn't passed as template argument would have to switch to vectors
    //for everything, generally I would say you don't need to decide the amount
    //of items in your combination at runtime, but rather it is a way to view 
    //a list based on the problem your solving, that being said it would be easy
    //to make it decided at runtime
#if 0
    template <typename Container>
        struct combinations_with_replacement_iter;

    template <typename Container>
        iterator_range<combinations_with_replacement_iter<Container>>
        combinations_with_replacement(Container & container, std::size_t N) {
            auto begin = combinations_with_replacement_iter<Container>(container, N);
            auto end = combinations_with_replacement_iter<Container>(container, N);
            return 
                iterator_range<combinations_with_replacement_iter<Container>>(begin,end);
        }
     template <typename T>
        iterator_range<combinations_with_replacement_iter<std::initializer_list<T>>>
        combinations_with_replacement(std::initializer_list<T> && container, std::size_t N) {
            auto begin = combinations_with_replacement_iter<std::initializer_list<T>>(container, N);
            auto end = combinations_with_replacement_iter<std::initializer_list<T>>(container, N);
            return {begin,end};
        }
#endif
    template <typename Container>
    class CombinatorWithReplacement {
        private:
            Container container;
            std::size_t length;

        public:
           CombinatorWithReplacement(Container container, std::size_t n)
               : container(std::forward<Container>(container)),
               length{n}
           { }

           class Iterator {
               private:
                   Container& items;
                   std::vector<iterator_type<Container>> indicies;
                   bool not_done;

               public:
                   using item_t =
                       typename std::remove_const<
                           typename std::remove_reference<
                               iterator_deref<Container>>::type>::type;

                   Iterator(
                           Container& container, std::size_t n)
                       : items(container),
                       indicies(n, std::begin(items)),
                       not_done{n != 0}
                   { }

                   std::vector<item_t> operator*()
                   {
                       std::vector<item_t> values;
                       for (auto i : indicies) {
                           values.push_back(*i);
                       }
                       return values;
                   }


                   Iterator& operator++() {
                       for (auto iter = indicies.rbegin(); iter != indicies.rend(); ++iter) {
                           ++(*iter);
                           if (*iter == std::end(items)) {
                               if ( (iter + 1) != indicies.rend()) {
                                   for (auto down = iter; down != indicies.rbegin()-1;--down) {
                                       (*down) = (*(iter + 1)) + 1; 
                                   }
                               }
                               else {
                                   not_done = false;
                                   break;
                               }
                           }
                           else break; 
                           //we break because none of the rest of the items need to 
                           //be incremented
                       }
                       return *this;
                   }

                   bool operator !=(const Iterator&) const
                   {
                       //because of the way this is done you have to start from the 
                       //begining of the range and end at the end, you could break in 
                       //the middle of the loop though, it's not different from the way
                       //that python's works
                       return not_done;
                   }
           };

           Iterator begin() {
               return {this->container, this->length};
           }

           Iterator end() {
               return {this->container, 0};
           }
    };

    template <typename Container>
    CombinatorWithReplacement<Container> combinations_with_replacement(
            Container&& container, std::size_t length) {
        return {std::forward<Container>(container), length};
    }

}

#endif //COMBINATIONS_WITH_REPLACEMENT_HPP
