#ifndef FILTERFALSE__H__
#define FILTERFALSE__H__

#include "filter.hpp"

namespace iter {

    //Forward declarations of FilterFalse and filterfalse
    template <typename FilterFunc, typename Container>
    class FilterFalse;

    template <typename FilterFunc, typename Container>
    FilterFalse<FilterFunc, Container> filterfalse(FilterFunc, Container &);

    template <typename FilterFunc, typename Container>
    class FilterFalse : public Filter<FilterFunc, Container> {
        // The filterfalse function is the only thing allowed to
        // create a FilterFalse
        friend FilterFalse filterfalse<FilterFunc, Container>(
                FilterFunc, Container &);
        using Base = Filter<FilterFunc, Container>;

        public:
            using Filter<FilterFunc, Container>::Filter;

            class Iterator {
                protected:
                    typename Base::contained_iter_type sub_iter;
                    const typename Base::contained_iter_type sub_end;
                    FilterFunc filter_func;

                    // skip every element that is true ender the predicate
                    void skip_passes() { 
                        while (this->sub_iter != this->sub_end
                                && this->filter_func(*this->sub_iter)) {
                            ++this->sub_iter;
                        }
                    }

                public:
                    Iterator (typename Base::contained_iter_type iter,
                            typename Base::contained_iter_type end,
                            FilterFunc filter_func) :
                        sub_iter(iter),
                        sub_end(end),
                        filter_func(filter_func)
                    { 
                        this->skip_passes();
                    } 

                    typename Base::contained_iter_ret operator*() const {
                        return *this->sub_iter;
                    }

                    Iterator & operator++() { 
                        ++this->sub_iter;
                        this->skip_passes();
                        return *this;
                    }

                    bool operator!=(const Iterator & other) const {
                        return this->sub_iter != other.sub_iter;
                    }
            };

            Iterator begin() const {
                return Iterator(
                        this->container.begin(),
                        this->container.end(),
                        this->filter_func);
            }

            Iterator end() const {
                return Iterator(
                        this->container.end(),
                        this->container.end(),
                        this->filter_func);
            }
    };


    template <typename FilterFunc, typename Container>
    FilterFalse<FilterFunc, Container> filterfalse(FilterFunc filter_func,
            Container & container) {
        return FilterFalse<FilterFunc, Container>(filter_func, container);
    }
}



#endif //ifndef FILTERFALSE__H__
