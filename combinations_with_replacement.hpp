#ifndef COMBINATIONS_WITH_REPLACEMENT_HPP
#define COMBINATIONS_WITH_REPLACEMENT_HPP

#include <array>
#include <vector>
#include <iterator_range.hpp>
#include <type_traits>

namespace iter {
    //if size isn't passed as template argument would have to switch to vectors
    //for everything, generally I would say you don't need to decide the amount
    //of items in your combination at runtime
    template <typename Container, size_t N>
        struct combinations_with_replacement_iter;

    template <size_t N, typename Container>
        iterator_range<combinations_with_replacement_iter<Container,N>>
        combinations_with_replacement(const Container & container) {
            auto begin = combinations_with_replacement_iter<Container,N>(container);
            auto end = combinations_with_replacement_iter<Container,N>(container);
            return 
                iterator_range<combinations_with_replacement_iter<Container,N>>(begin,end);
        }
    template <typename Container, size_t N>
        struct combinations_with_replacement_iter
        {
        private:
            const Container & items;
            std::array<decltype(items.cbegin()),N> indicies;
            //std::vector<decltype(items.cbegin())> indicies;
            bool not_done = true;

        public:
            using item_t = typename std::remove_const<decltype(*(items.cbegin()))>::type;
            //combinations_with_replacement_iter() {} 
            //default constructor to avoid constructor for dummy iter
            combinations_with_replacement_iter(const Container & i) : 
                items(i) 
            {
                    for (auto & iter : indicies) iter = items.cbegin();
            }

            //make this an std::array, to lazy right now
            //using iter_t = decltype(indicies.begin());
            /*
            std::array<item_t,N> operator* () const
            {
                std::array<item_t,N> values;
                auto iter = values.begin();
                for (auto i : indicies) {
                    iter= *i;
                    ++iter;
                }
                return values;
            }
            */
            std::array<decltype(items.cbegin()),N>
            operator*()const
            {
                return indicies;
            }
            //int operator* () const {return 0;}


            combinations_with_replacement_iter &
            operator++() 
            {
                for (auto iter = indicies.rbegin(); iter != indicies.rend(); ++iter) {
                    ++(*iter);
                    if (*iter == items.cend()) {
                        if ( (iter + 1) != indicies.rend()) {
                            //(*iter) = (*(iter + 1)) + 1; 
                            //this was wrong actually have to go down the whole 
                            //list setting stuff
                            for (auto down = iter; down != indicies.rbegin()-1;--down) {
                                (*down) = (*(iter + 1)) + 1; 
                            }
                            //*(iter+1) could be derefing a past the end iter
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

            bool operator !=(const combinations_with_replacement_iter &)
            {
                return not_done;
            }
        };
}

#endif //COMBINATIONS_WITH_REPLACEMENT_HPP
