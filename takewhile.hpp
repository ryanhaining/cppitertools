#ifndef TAKEWHILE__H__
#define TAKEWHILE__H__

#include <iterbase.hpp>

#include <utility>
#include <iterator>

namespace iter {

    //Forward declarations of TakeWhile and takewhile
    template <typename FilterFunc, typename Container>
    class TakeWhile;

    template <typename FilterFunc, typename Container>
    TakeWhile<FilterFunc, Container> takewhile(FilterFunc, Container &);

    template <typename FilterFunc, typename Container>
    class TakeWhile : IterBase<Container>{
        private:
            Container & container;
            FilterFunc filter_func;

            friend TakeWhile takewhile<FilterFunc, Container>(
                    FilterFunc, Container &);

            using typename IterBase<Container>::contained_iter_type;

            using typename IterBase<Container>::contained_iter_ret;

            // Value constructor for use only in the takewhile function
            TakeWhile(FilterFunc filter_func, Container & container) :
                container(container),
                filter_func(filter_func)
            { }

            TakeWhile () = delete;
            TakeWhile & operator=(const TakeWhile &) = delete;

        public:
            TakeWhile(const TakeWhile &) = default;

            class Iterator {
                private:
                    contained_iter_type sub_iter;
                    const contained_iter_type sub_end;
                    FilterFunc filter_func;

                    // increment until the iterator points to is true on the 
                    // predicate.  Called by constructor and operator++
                    void check_current() {
                        if (!this->filter_func(*this->sub_iter)) {
                            this->sub_iter = this->sub_end;
                        }
                    }

                public:
                    Iterator (contained_iter_type iter,
                            contained_iter_type end,
                            FilterFunc filter_func) :
                        sub_iter(iter),
                        sub_end(end),
                        filter_func(filter_func)
                    { 
                        this->check_current();
                    } 

                    contained_iter_ret operator*() const {
                        return *this->sub_iter;
                    }

                    Iterator & operator++() { 
                        ++this->sub_iter;
                        this->check_current();
                        return *this;
                    }

                    bool operator!=(const Iterator & other) const {
                        return this->sub_iter != other.sub_iter;
                    }
            };

            Iterator begin() const {
                return Iterator(
                        std::begin(this->container),
                        std::end(this->container),
                        this->filter_func);
            }

            Iterator end() const {
                return Iterator(
                        std::end(this->container),
                        std::end(this->container),
                        this->filter_func);
            }

    };

    // Helper function to instantiate a TakeWhile
    template <typename FilterFunc, typename Container>
    TakeWhile<FilterFunc, Container> takewhile(
            FilterFunc filter_func, Container & container) {
        return TakeWhile<FilterFunc, Container>(filter_func, container);
    }

}

#endif //ifndef TAKEWHILE__H__
