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


    //Forward declarations of filterfalseFalseDefault and filterfalse
    template <typename Container>
    class filterfalseFalseDefault;

    template <typename Container>
    filterfalseFalseDefault<Container> filterfalse(Container &);

    template <typename Container>
    class filterfalseFalseDefault {
        protected:
            Container & container;

            // The filterfalse function is the only thing allowed to create a
            // filterfalseFalseDefault
            friend filterfalseFalseDefault filterfalse<Container>(Container &);
            // Type of the Container::Iterator, but since the name of that 
            // iterator can be anything, we have to grab it with this
            using contained_iter_type = decltype(container.begin());

            // The type returned when dereferencing the Container::Iterator
            using contained_iter_ret = decltype(container.begin().operator*());

            // Value constructor for use only in the filterfalse function
            filterfalseFalseDefault(Container & container) :
                container(container)
            { }
            filterfalseFalseDefault () = delete;
            filterfalseFalseDefault & operator=(const filterfalseFalseDefault &) = delete;

        public:
            filterfalseFalseDefault(const filterfalseFalseDefault &) = default;
            class Iterator {
                protected:
                    contained_iter_type sub_iter;
                    const contained_iter_type sub_end;

                    void skip_passes() { 
                        while (this->sub_iter != this->sub_end
                                && *this->sub_iter) {
                            ++this->sub_iter;
                        }
                    }

                public:
                    Iterator (contained_iter_type iter,
                            contained_iter_type end) :
                        sub_iter(iter),
                        sub_end(end)
                    { 
                        this->skip_passes();
                    } 

                    contained_iter_ret operator*() const {
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
                        this->container.end());
            }

            Iterator end() const {
                return Iterator(
                        this->container.end(),
                        this->container.end());
            }

    };

    // Helper function to instantiate a filterfalseFalseDefault
    template <typename Container>
    filterfalseFalseDefault<Container> filterfalse(Container & container) {
        return filterfalseFalseDefault<Container>(container);
    }
}



#endif //ifndef FILTERFALSE__H__
