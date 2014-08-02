#ifndef COMBINATIONS_WITH_REPLACEMENT_HPP
#define COMBINATIONS_WITH_REPLACEMENT_HPP

#include "iterator_range.hpp"
#include "iterbase.hpp"

#include <vector>
#include <type_traits>

namespace iter {

    template <typename Container>
    class CombinatorWithReplacement;

    template <typename Container>
    CombinatorWithReplacement<Container> combinations_with_replacement(
            Container&&, std::size_t);

    template <typename T>
    CombinatorWithReplacement<std::initializer_list<T>>
    combinations_with_replacement(
            std::initializer_list<T>, std::size_t);

    template <typename Container>
    class CombinatorWithReplacement {
        private:
            Container container;
            std::size_t length;

            friend CombinatorWithReplacement
                combinations_with_replacement<Container>(
                        Container&& ,std::size_t);
            template <typename T>
            friend CombinatorWithReplacement<std::initializer_list<T>>
            combinations_with_replacement(
                    std::initializer_list<T>, std::size_t);

           CombinatorWithReplacement(Container container, std::size_t n)
               : container(std::forward<Container>(container)),
               length{n}
           { }

        public:
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

    template <typename T>
    CombinatorWithReplacement<std::initializer_list<T>> 
    combinations_with_replacement(
            std::initializer_list<T> il, std::size_t length) {
        return {il, length};
    }
}

#endif //COMBINATIONS_WITH_REPLACEMENT_HPP
