#ifndef ITER_COMBINATIONS_WITH_REPLACEMENT_HPP_
#define ITER_COMBINATIONS_WITH_REPLACEMENT_HPP_

#include "iterbase.hpp"

#include <iterator>
#include <vector>
#include <type_traits>
#include <initializer_list>

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

           CombinatorWithReplacement(Container&& container, std::size_t n)
               : container(std::forward<Container>(container)),
               length{n}
           { }

            using CombIteratorDeref =
                std::vector<collection_item_type<Container>>;

        public:
            class Iterator :
                public std::iterator<std::input_iterator_tag,
                    CombIteratorDeref>
            {
               private:
                   constexpr static const int COMPLETE = -1;
                   typename std::remove_reference<Container>::type *container_p;
                   std::vector<iterator_type<Container>> indicies;
                   CombIteratorDeref working_set;
                   int steps;

                   void compute_working_set() {
                       this->working_set.clear();
                       for (auto&& i : indicies) {
                           this->working_set.emplace_back(*i);
                       }
                   }

               public:
                   Iterator(Container& in_container, std::size_t n)
                       : container_p{&in_container},
                       indicies(n, std::begin(in_container)),
                       steps{(std::begin(in_container)
                               != std::end(in_container)
                               && n)
                           ? 0 : COMPLETE}
                   {
                       this->compute_working_set();
                   }

                   CombIteratorDeref& operator*() {
                       return this->working_set;
                   }


                   Iterator& operator++() {
                       for (auto iter = indicies.rbegin();
                               iter != indicies.rend();
                               ++iter) {
                           ++(*iter);
                           if (!(*iter != std::end(*this->container_p))) {
                               if ( (iter + 1) != indicies.rend()) {
                                   for (auto down = iter;
                                           down != indicies.rbegin()-1;
                                           --down) {
                                       (*down) = dumb_next(*(iter + 1)); 
                                   }
                               } else {
                                   this->steps = COMPLETE;
                                   break;
                               }
                           } else {
                               //we break because none of the rest of the items
                               //need to be incremented
                               break; 
                           }
                       }
                       if (this->steps != COMPLETE) {
                           ++this->steps;
                           this->compute_working_set();
                       }
                       return *this;
                   }


                   Iterator operator++(int) {
                       auto ret = *this;
                       ++*this;
                       return ret;
                   }

                   bool operator!=(const Iterator& other) const {
                       return !(*this == other);
                   }

                   bool operator==(const Iterator& other) const {
                       return this->steps == other.steps;
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
        return {std::move(il), length};
    }
}

#endif
