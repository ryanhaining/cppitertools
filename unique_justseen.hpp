#ifndef UNIQUE_JUSTSEEN_HPP
#define UNIQUE_JUSTSEEN_HPP

#include "groupby.hpp"
#include "imap.hpp"

#include <type_traits>
#include <functional>
#include <utility>

namespace iter 
{
    // gets first of each group.  since each group is decided based on equality
    // with the previous item, this results in each item only appearing once
    template <typename Container>
    auto unique_justseen(Container&& container) {
        return imap(
                [] (iterator_deref<
                        decltype(
                            groupby(
                                std::forward<Container>(container)))>&& gb)
                    {return *std::begin(gb.second);},
                groupby(std::forward<Container>(container)));
    }
}

#endif //UNIQUE_JUSTSEEN_HPP
    
