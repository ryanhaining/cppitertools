#ifndef ITER_CHAIN_HPP_
#define ITER_CHAIN_HPP_

#include "iterbase.hpp"

#include <utility>
#include <iterator>
#include <memory>
#include <initializer_list>
#include <type_traits>
#include <tuple>
#include <array>

namespace iter {
    // rather than a chain function, use a callable object to support
    // from_iterable
    class ChainMaker;

    template <typename TupleType, std::size_t... Is>
    class Chained {
        friend class ChainMaker;

        private:
            class ChainIterWrapperBase {
                protected:
                    using ResultType =
                        iterator_deref<std::tuple_element_t<0, TupleType>>;
                public:
                    virtual ~ChainIterWrapperBase() { }
                    virtual bool operator!=(
                            const ChainIterWrapperBase&) const = 0;
                    virtual bool operator==(
                            const ChainIterWrapperBase&) const = 0;
                    virtual ResultType operator*() = 0;
                    virtual ChainIterWrapperBase& operator++() = 0;
            };

            template <typename Container>
            class ChainIterWrapper : public ChainIterWrapperBase {
                private:
                    iterator_type<Container> sub_iter;

                public:
                    ChainIterWrapper(const iterator_type<Container>& iter)
                        : sub_iter{iter}
                    { }

                    bool operator!=(const ChainIterWrapperBase& other)
                            const override {
                        return this->sub_iter !=
                            static_cast<const ChainIterWrapper&>(
                                    other).sub_iter;
                    }

                    bool operator==(const ChainIterWrapperBase& other)
                            const override {
                        return !(*this !=
                                static_cast<const ChainIterWrapper&>(other));
                    }

                    typename ChainIterWrapper::ResultType operator*() {
                        return *this->sub_iter;
                    }

                    ChainIterWrapper& operator++() override {
                        ++this->sub_iter;
                        return *this;
                    }
            };


        private:
            using ArrayType =
                std::array<std::unique_ptr<ChainIterWrapperBase>,
                sizeof...(Is)>;
            TupleType containers;
            ArrayType end_iter_wrappers;

            Chained(TupleType&& tup_containers)
                : containers(std::move(tup_containers)),
                end_iter_wrappers{{std::make_unique<
                    ChainIterWrapper<std::tuple_element_t<Is, TupleType>>>(
                            std::end(std::get<Is>(this->containers)))...}}
            { }

        public:
            class Iterator {
                private:
                    using ArrayType =
                        std::array<std::unique_ptr<ChainIterWrapperBase>,
                        sizeof...(Is)>;

                    ArrayType iter_wrappers;
                    const ArrayType& end_iter_wrappers;
                    std::size_t index;

                    void check_index() {
                        while (this->index < iter_wrappers.size()
                                && *this->iter_wrappers[this->index]
                                    == *this->end_iter_wrappers[this->index]) {
                            ++this->index;
                        }
                    }


                public:
                    Iterator(ArrayType&& iters, ArrayType& ends, std::size_t i)
                        : iter_wrappers(std::move(iters)),
                        end_iter_wrappers{ends},
                        index{i}
                    {
                        this->check_index();
                    }

                    Iterator(const Iterator& other) 
                        : iter_wrappers{{
                            // make this not so awful.
                            std::unique_ptr<ChainIterWrapperBase>{new ChainIterWrapper<std::tuple_element_t<Is, TupleType>>(dynamic_cast<ChainIterWrapper<std::tuple_element_t<Is, TupleType>>&>(*std::get<Is>(other.iter_wrappers)))}...
                        }},
                        end_iter_wrappers{other.end_iter_wrappers},
                        index{other.index}
                    { }

                    Iterator& operator++() {
                        ++*this->iter_wrappers[this->index];
                        this->check_index();
                        return *this;
                    }

                    decltype(auto) operator*() {
                        return **this->iter_wrappers[this->index];
                    }

                    bool operator!=(const Iterator& other) const {
                        return this->index != other.index;
                    }
            };


            Iterator begin() {
                ArrayType a{{
                    std::make_unique<
                        ChainIterWrapper<std::tuple_element_t<Is, TupleType>>>(
                                std::begin(std::get<Is>(this->containers))
                            )...}};
                return {std::move(a), end_iter_wrappers, 0};
            }

            Iterator end() {
                ArrayType a{{std::unique_ptr<
                    ChainIterWrapper<std::tuple_element_t<Is, TupleType>>>{
                        nullptr}...}};
                return {std::move(a), end_iter_wrappers, sizeof...(Is)};
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
        private:
            template <typename TupleType, std::size_t... Is>
            Chained<TupleType, Is...> chain_impl(
                    TupleType&& in_containers,
                    std::index_sequence<Is...>) const {
                return {std::move(in_containers)};
            }

        public:
            // expose regular call operator to provide usual chain()
            template <typename... Containers>
            auto operator()(Containers&&... cs) const {
                return this->chain_impl(
                        std::tuple<Containers...>{
                            std::forward<Containers>(cs)...},
                        std::index_sequence_for<Containers...>{});
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

#endif // #ifndef ITER_CHAIN_HPP_
