#ifndef ITER_STARMAP_H_
#define ITER_STARMAP_H_

#include "iterbase.hpp"

#include <utility>
#include <type_traits>
#include <array>
#include <cassert>
#include <memory>

namespace iter {
    // starmap with a container<T> where T is one of tuple, pair, array
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
    StarMapper<Func, Container> starmap_helper(
            Func func, Container&& container, std::false_type) {
        return {func, std::forward<Container>(container)};
    }

    // starmap for a tuple or pair of tuples or pairs
    template <typename Func, typename TupleType,
             std::size_t Size =std::tuple_size<std::decay_t<TupleType>>::value>
    class TupleStarMapper {
        private:
            class TupleExpanderBase {
                protected:
                    // deduced return type to return of Func when called with
                    // one of TupleType
                    using ResultType = 
                        decltype(call_with_tuple(
                                    std::declval<Func&>(),
                                    std::get<0>(std::declval<TupleType&>())));
                public:
                    virtual ResultType call() = 0;

                    virtual ~TupleExpanderBase() { }
            };

            template <std::size_t Idx>
            class TupleExpander : public TupleExpanderBase {
                private:
                    Func func;
                    TupleType& tup;
                public:
                    TupleExpander(Func f, TupleType& t)
                        : func(f),
                        tup(t)
                    { }

                    typename TupleExpanderBase::ResultType call() override {
                        return call_with_tuple(
                                this->func, std::get<Idx>(this->tup));
                    }
            };

        private:
            Func func;
            TupleType tup;
            std::array<std::unique_ptr<TupleExpanderBase>, Size> tuple_getters;

            template <std::size_t... Is>
            TupleStarMapper(Func f, TupleType t, std::index_sequence<Is...>)
                : func(f),
                tup(std::forward<TupleType>(t)),
                tuple_getters(
                        {std::make_unique<TupleExpander<Is>>(func, tup)...})
            { }

        public:
            TupleStarMapper(Func f, TupleType t)
                : TupleStarMapper(f, std::forward<TupleType>(t),
                    std::make_index_sequence<Size>{})
            { }

            class Iterator {
                private:
                    std::array<std::unique_ptr<TupleExpanderBase>, Size>&
                        tuple_getters;
                    std::size_t index;

                public:
                    Iterator(std::array<
                                std::unique_ptr<TupleExpanderBase>, Size>& tg,
                            std::size_t i)
                        : tuple_getters(tg),
                        index{i}
                    { }

                    decltype(auto) operator*() {
                        return this->tuple_getters[this->index]->call();
                    }

                    Iterator operator++() {
                        ++this->index;
                        return *this;
                    }

                    bool operator!=(const Iterator& other) const {
                        return this->index != other.index;
                    }
            };

            Iterator begin() {
                return {this->tuple_getters, 0};
            }

            Iterator end() {
                return {this->tuple_getters, Size};
            }
    };

    template <typename Func, typename TupleType>
    TupleStarMapper<Func, TupleType> starmap_helper(
            Func func, TupleType&& tup, std::true_type) {
        return {func, std::forward<TupleType>(tup)};
    }

    // "tag dispatch" to differentiate between normal containers and
    // tuple-like containers, things that work with std::get
    template <typename T, typename U =void>
    struct is_tuple_like : public std::false_type { };

    template <typename T>
    struct is_tuple_like<T, decltype(std::get<0>(std::declval<T>()), void())>
        : public std::true_type { };

    template <typename Func, typename Seq>
    auto starmap(Func func, Seq&& sequence) {
        return starmap_helper(
                func, std::forward<Seq>(sequence),
                is_tuple_like<Seq>{});
    }
}




#endif // #ifndef ITER_STARMAP_H_
