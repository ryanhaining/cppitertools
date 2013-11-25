#ifndef UNIQUE_EVERSEEN_HPP
#define UNIQUE_EVERSEEN_HPP

#include "filter.hpp"

#include <type_traits>
#include <functional>
#include <utility>
#include <unordered_map>

namespace iter 
{
    //the container type must be usable in an unordered_map to achieve constant
    //performance checking if it has ever been seen
    template <typename Container>
    auto unique_everseen(Container && container) 
    -> Filter<std::function<bool(decltype(container.front()))>,Container>
    {
        using elem_t = decltype(container.front());
        std::unordered_map<typename std::remove_reference<elem_t>::type,bool> elem_seen;
        std::function<bool(elem_t)> func = [elem_seen](elem_t e) mutable
        //has to be captured by value because it goes out of scope when the 
        //function returns
                {
                    if(!elem_seen[e]) {
                        elem_seen[e] = true;
                        return true;
                    }
                    else return false;
                };
        return filter(func,std::forward<Container>(container));
    }
}

#endif //UNIQUE_EVERSEEN_HPP
