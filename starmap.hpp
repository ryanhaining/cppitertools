#ifndef ITER_STARMAP_H_
#define ITER_STARMAP_H_

#include "iterbase.hpp"

#include <utility>
#include <cassert>

namespace iter {
    template <typename Func, typename Container>
    class StarMapper {
        private:
            Func func;
            Container container;
        public:
            StarMapper(Func f, Container c)
                : func(f),
                container(std::forward<Container>(c))
            { }

            class Iterator {
                private:
                    Func func;
                    iterator_type<Container> sub_iter;
                public:
                    Iterator(Func f, iterator_type<Container> iter)
                        : func(f),
                        sub_iter(iter)
                    { }

                    bool operator!=(const Iterator& other) const {
                        return this->sub_iter != other.sub_iter;
                    }

                    Iterator operator++() {
                        ++this->sub_iter;
                        return *this;
                    }

                    decltype(auto) operator*() {
                        return call_with_tuple(this->func, *this->sub_iter);
                    }
            };

            Iterator begin() {
                return {this->func, std::begin(this->container)};
            }

            Iterator end() {
                return {this->func, std::end(this->container)};
            }
    };

    template <typename Func, typename Container>
    StarMapper<Func, Container> starmap(Func func, Container&& container) {
        return {func, std::forward<Container>(container)};
    }

    template <typename Func, typename TupleType,
             std::size_t Size =std::tuple_size<std::decay_t<TupleType>>::value>
    class TupleStarMapper {
        private:
            Func func;
            TupleType tup;
        public:
            TupleStarMapper(Func f, TupleType t)
                : func(f),
                tup(std::forward<TupleType>(t))
            { }

            template <std::size_t Dummy, std::size_t Idx>
            class Iterator {
                private:
                    Func func;
                    TupleType& tup;
                    bool passed;
                    Iterator<0, Idx + 1> next_level;

                public:
                    Iterator(Func f, TupleType& t, bool done)
                        : func(f),
                        tup(t),
                        passed{done},
                        next_level(f, t, done)
                    { }

                    decltype(auto) operator*() {
                        if (this->passed) {
                            return *this->next_level;
                        } else {
                            return call_with_tuple(
                                    this->func, std::get<Idx>(this->tup));
                        }
                    }

                    Iterator operator++() {
                        if (!this->passed) {
                            this->passed = true;
                        } else {
                            ++this->next_level;
                        }
                        return *this;
                    }

                    bool operator!=(const Iterator& other) const {
                        return this->passed != other.passed
                            || this->next_level != other.next_level;
                    }
            };

            template <std::size_t Dummy>
            class Iterator<Dummy, Size> {
                private:
                    // data members unused since this should never get
                    // dereferenced, but are needed to compile
                    Func func;
                    TupleType& tup;
                public:
                    Iterator(Func f, TupleType& t, bool)
                        : func(f),
                        tup(t)
                    { }
                    
                    decltype(auto) operator*() {
                        assert(false && "deref of last level in starmap");
                        return call_with_tuple(func, std::get<0>(tup));
                    }

                    Iterator operator++() {
                        assert(false && "++ on last level of starmap");
                        return *this;
                    }

                    bool operator!=(const Iterator&) const {
                        return false;
                    }
            };

            Iterator<0, 0> begin() {
                return {this->func, this->tup, false};
            }

            Iterator<0, 0> end() {
                return {this->func, this->tup, true};
            }
    };

    template <typename Func, typename... Ts>
    TupleStarMapper<Func, std::tuple<Ts...>> starmap(
            Func func, std::tuple<Ts...> tup) {
        return {func, tup};
    }
    

}




#endif // #ifndef ITER_STARMAP_H_
