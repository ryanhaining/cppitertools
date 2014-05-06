#ifndef CHAIN_HPP
#define CHAIN_HPP

#include "iterator_range.hpp"

#include <utility>
#include <iterator>

namespace iter {
    template <typename ... Containers>
        struct chain_iter;
    template <typename Container>
        struct chain_iter<Container> {

            private:
                //using Iterator = decltype(std::declval<Container>().begin());
                using Iterator = decltype(std::begin(std::declval<Container>()));
                Iterator begin;
                const Iterator end;//never really used but kept it for consistency

            public:
	  using value_type = typename std::iterator_traits<Iterator>::value_type;
	  using difference_type = ptrdiff_t; //not sure when this gets used...
	  using pointer = value_type*;
	  using reference = value_type&;
	  using iterator_category = std::forward_iterator_tag; //could probably be made bidirectional
	  
	  
                chain_iter(Container && container, bool is_end=false) :
                    begin(std::begin(container)),end(std::end(container)) {
                        if(is_end) begin = std::end(container);
                    }
                chain_iter & operator++()
                {
                    ++begin;
                    return *this;
                }
                auto operator*()->decltype(*begin)
                {
                    return *begin;
                }
                bool operator!=(const chain_iter & rhs) const{
                    return this->begin != rhs.begin;
                }
        };
    template <typename Container, typename ... Containers>
        struct chain_iter<Container,Containers...>
        {
            private:
                //using Iterator = decltype(std::declval<Container>().begin());
                using Iterator = decltype(std::begin(std::declval<Container>()));
                Iterator begin;
                const Iterator end;
                bool end_reached = false;
                chain_iter<Containers...> next_iter;

            public:
		  using value_type = typename std::iterator_traits<Iterator>::value_type;
		  using difference_type = ptrdiff_t; //not sure when this gets used...
		  using pointer = value_type*;
		  using reference = value_type&;
		  using iterator_category = std::forward_iterator_tag; //could probably be made bidirectional

                chain_iter(Container && container, Containers&& ... containers, bool is_end=false) :
                    begin(std::begin(container)),
                    end(std::end(container)),
                    next_iter(std::forward<Containers>(containers)...,is_end) {
                        if(is_end)
                            begin = std::end(container);
                    }
                chain_iter & operator++()
                {
                    if (!(begin != end)) {
                        ++next_iter;
                    }
                    else {
                        ++begin;
                    }
                    return *this;               
                }
                auto operator*()->decltype(*begin)
                {
                    if (!(begin != end)) {
                        return *next_iter;
                    }
                    else {
                        return *begin;
                    }
                }   
                bool operator !=(const chain_iter & rhs) const {
                    if (!(begin != end)) {
                        return this->next_iter != rhs.next_iter;
                    }
                    else
                        return this->begin != rhs.begin;
                }
        };
    
    template <typename ... Containers>
        iterator_range<chain_iter<Containers...>> chain(Containers&& ... containers)
        {
            auto begin = 
                chain_iter<Containers...>(std::forward<Containers>(containers)...);
            auto end =
                chain_iter<Containers...>(std::forward<Containers>(containers)...,true);
            return 
                iterator_range<chain_iter<Containers...>>(begin,end);
        }
}
/*
  //put these as using decs in the iterator itself
namespace std {
    template <typename ... Containers>
        struct iterator_traits<iter::chain_iter<Containers...>> {
            using difference_type = ptrdiff_t;
            using iterator_category = input_iterator_tag;
        };
		}*/
#endif //CHAIN_HPP
