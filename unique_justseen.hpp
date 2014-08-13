#ifndef UNIQUE_JUSTSEEN_HPP
#define UNIQUE_JUSTSEEN_HPP

#include "iterbase.hpp"
#include "groupby.hpp"
#include "imap.hpp"

#include <utility>
#include <iterator>
#include <initializer_list>

namespace iter 
{
    template <typename GroupByType>
    struct GroupFrontGetter{
        auto operator()(iterator_deref<GroupByType>&& gb) ->
                decltype(*std::begin(gb.second)) {
            return *std::begin(gb.second);
        }

        auto operator()(iterator_deref<GroupByType>& gb) ->
                decltype(*std::begin(gb.second)) {
            return *std::begin(gb.second);
        }
    };


    // gets first of each group.  since each group is decided based on equality
    // with the previous item, this results in each item only appearing once
    template <typename Container>
    auto unique_justseen(Container&& container) ->
        decltype(imap(GroupFrontGetter<decltype(
                        groupby(std::forward<Container>(container)))>{},
                groupby(std::forward<Container>(container)))) {
        return imap(GroupFrontGetter<decltype(
                    groupby(std::forward<Container>(container)))>{},
                groupby(std::forward<Container>(container)));
    }

    template <typename T>
    auto unique_justseen(std::initializer_list<T> il) ->
        decltype(imap(GroupFrontGetter<decltype(
                        groupby(std::forward<std::initializer_list<T>>(il)))>{},
                groupby(std::forward<std::initializer_list<T>>(il)))) {
        return imap(GroupFrontGetter<decltype(
                    groupby(std::forward<std::initializer_list<T>>(il)))>{},
                groupby(std::forward<std::initializer_list<T>>(il)));
    }
}

#endif //UNIQUE_JUSTSEEN_HPP
    
