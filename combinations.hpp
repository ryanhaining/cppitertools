#ifndef COMBINATIONS_HPP
#define COMBINATIONS_HPP

#include <array>
#include <vector>
#include <iterator_range.hpp>
#include <type_traits>
#include <iterator>
#include <iostream>

namespace iter {
    //if size isn't passed as template argument would have to switch to vectors
    //for everything, generally I would say you don't need to decide the amount
    //of items in your combination at runtime, but rather it is a way to view 
    //a list based on the problem your solving, that being said it would be easy
    //to make it decided at runtime
    template <typename Container>
        struct combinations_iter;

    template <typename Container>
        iterator_range<combinations_iter<Container>>
        combinations(const Container & container, size_t N) {
            auto begin = combinations_iter<Container>(container,N);
            auto end = combinations_iter<Container>(container,N);
            return 
                iterator_range<combinations_iter<Container>>(begin,end);
        }
    template <typename Container>
        struct combinations_iter
        {
        private:
            const Container & items;
            std::vector<decltype(items.cbegin())> indicies;
            bool not_done = true;

        public:
            //Holy shit look at this typedef
            using item_t = typename 
                std::remove_const<
                typename std::remove_reference<decltype(items.front())>::type>::type;
            combinations_iter(const Container & i, size_t N) : 
                items(i),indicies(N)
            {
                size_t inc = 0;
                for (auto & iter : indicies) {
                    if (items.cbegin() + inc != items.cend()) {
                        iter = items.cbegin()+inc;
                        ++inc;
                    }
                    else {
                        not_done = false;
                        break;
                    }
                }
            }

            std::vector<item_t> operator*() const
            {
                std::vector<item_t> values;
                for (auto i : indicies) {
                    values.push_back(*i);
                }
                return values;
            }


            combinations_iter &
            operator++() 
            {
                for (auto iter = indicies.rbegin(); iter != indicies.rend(); ++iter) {
                    ++(*iter);
                    //what we have to check here is if the distance between the
                    //index and the end of indicies is >= the distance between
                    //the item and end of item
                    if ((*iter + std::distance(indicies.rbegin(),iter)) ==
                            items.cend()) {
                        if ( (iter + 1) != indicies.rend()) {
                            size_t inc = 1;
                            for (auto down = iter; down != indicies.rbegin()-1;--down) {
                                (*down) = (*(iter + 1)) + 1 + inc;
                                /*if (*down == items.cend()) {
                                    iter = iter + 1;
                                }*/
                                ++inc;
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

            bool operator !=(const combinations_iter &)
            {
                //because of the way this is done you have to start from the 
                //begining of the range and end at the end, you could break in 
                //the middle of the loop though, it's not different from the way
                //that python's works
                return not_done;
            }
        };
}

#endif //COMBINATIONS_HPP
