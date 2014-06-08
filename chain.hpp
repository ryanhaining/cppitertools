#ifndef CHAIN__HPP__
#define CHAIN__HPP__

#include "iterbase.hpp"

#include <utility>
#include <iterator>
#include <memory>
#include <initializer_list>
#include <type_traits>

namespace iter {
    // rather than a chain function, use a callable object to support
    // from_iterable
    class ChainMaker;

    template <typename Container, typename... RestContainers>
    class Chained {
        friend class ChainMaker;
        template <typename C, typename... RC>
        friend class Chained;

        private:
            Container container;
            Chained<RestContainers...> rest_chained;
            Chained(Container container, RestContainers&&... rest)
                : container(std::forward<Container>(container)),
                rest_chained{std::forward<RestContainers>(rest)...}
            { }

        public:
            class Iterator {
                private:
                    using RestIter =
                        typename Chained<RestContainers...>::Iterator;
                    iterator_type<Container> sub_iter;
                    const iterator_type<Container> sub_end;
                    RestIter rest_iter;
                    bool at_end;

                public:
                    Iterator(const iterator_type<Container>& s_begin,
                            const iterator_type<Container>& s_end,
                            RestIter rest_iter)
                        : sub_iter{s_begin},
                        sub_end{s_end},
                        rest_iter{rest_iter},
                        at_end{!(sub_iter != sub_end)}
                    { }
                    
                    Iterator& operator++() {
                        if (this->at_end) {
                            ++this->rest_iter;
                        } else {
                            ++this->sub_iter;
                            if (!(this->sub_iter != this->sub_end)) {
                                this->at_end = true;
                            }
                        }
                        return *this;
                    }

                    bool operator!=(const Iterator& other) const {
                        return this->sub_iter != other.sub_iter ||
                            this->rest_iter != other.rest_iter;
                    }

                    iterator_deref<Container> operator*() {
                        return this->at_end ?
                            *this->rest_iter : *this->sub_iter;
                    }
            };

            Iterator begin() {
                return {std::begin(this->container),
                    std::end(this->container),
                    std::begin(this->rest_chained)};
            }

            Iterator end() {
                return {std::end(this->container),
                    std::end(this->container),
                    std::end(this->rest_chained)};
            }
    };
    template <typename Container>
    class Chained<Container> {
        friend class ChainMaker;
        template <typename C, typename... RC>
        friend class Chained;

        private:
            Container container;
            Chained(Container container)
                : container(std::forward<Container>(container))
            { }

        public:
            class Iterator {
                private:
                    iterator_type<Container> sub_iter;
                    const iterator_type<Container> sub_end;

                public:
                    Iterator(const iterator_type<Container>& s_begin,
                            const iterator_type<Container>& s_end)
                        : sub_iter{s_begin},
                        sub_end{s_end}
                    { }
                    
                    Iterator& operator++() {
                        ++this->sub_iter;
                        return *this;
                    }

                    bool operator!=(const Iterator& other) const {
                        return this->sub_iter != other.sub_iter;
                    }

                    iterator_deref<Container> operator*() {
                        return *this->sub_iter;
                    }
            };

            Iterator begin() {
                return {std::begin(this->container),
                    std::end(this->container)};
            }

            Iterator end() {
                return {std::end(this->container),
                    std::end(this->container)};
            }
    };

    template <typename Container>
    class ChainedFromIterable {
        private:
            Container container;
            friend class ChainMaker;
            ChainedFromIterable(Container container)
                : container(std::forward<Container>(container))
            { }

        public:
            class Iterator {
                private:
                    using SubContainer = iterator_deref<Container>;
                    using SubIter = iterator_type<SubContainer>;

                   iterator_type<Container> top_level_iter;
                   const iterator_type<Container> top_level_end;
                   std::unique_ptr<SubIter> sub_iter_p;
                   std::unique_ptr<SubIter> sub_end_p;
                public:
                   Iterator(iterator_type<Container> top_iter,
                           iterator_type<Container> top_end)
                       : top_level_iter{top_iter},
                       top_level_end{top_end},
                       sub_iter_p{!(top_iter != top_end) ?  // iter == end ?
                           nullptr : new SubIter{std::begin(*top_iter)}},
                       sub_end_p{!(top_iter != top_end) ?  // iter == end ?
                           nullptr : new SubIter{std::end(*top_iter)}}
                   { }

                   Iterator& operator++() {
                       ++*this->sub_iter_p;
                       if (!(*this->sub_iter_p != *this->sub_end_p)) {
                           ++this->top_level_iter;
                           if (this->top_level_iter != this->top_level_end) {
                               sub_iter_p.reset(
                                       new SubIter{std::begin(*this->top_level_iter)});
                               sub_end_p.reset(
                                       new SubIter{std::end(*this->top_level_iter)});
                           } else {
                               sub_iter_p.reset(nullptr);
                               sub_end_p.reset(nullptr);
                           }
                       }
                       return *this;
                   }
                       

                   bool operator!=(const Iterator& other) const {
                       return this->top_level_iter != other.top_level_iter &&
                           (this->sub_iter_p != other.sub_iter_p ||
                            *this->sub_iter_p != *other.sub_iter_p);
                   }

                   iterator_deref<iterator_deref<Container>> operator*() {
                       return **this->sub_iter_p;
                   }
           };

           Iterator begin() {
               return {std::begin(this->container), std::end(this->container)};
           }

           Iterator end() {
               return {std::end(this->container), std::end(this->container)};
           }
    };


    class ChainMaker {
        public:
            // expose regular call operator to provide usual chain()
            template <typename... Containers>
            Chained<Containers...> operator()(Containers&&... cs) const {
                return {std::forward<Containers>(cs)...};
            }

            // chain.from_iterable
            template <typename Container>
            ChainedFromIterable<Container> from_iterable(
                    Container&& container) const {
                return {std::forward<Container>(container)};
            }
    };

    namespace {
        constexpr auto chain = ChainMaker{};
    }

}

#endif //#define CHAIN__HPP__
