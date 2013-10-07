#ifndef FILTER__H__
#define FILTER__H__

#include <utility>

namespace iter {

    //Forward declarations of Filter and filter
    template <typename FilterFunc, typename Container>
    class Filter;

    template <typename FilterFunc, typename Container>
    Filter<FilterFunc, Container> filter(FilterFunc, Container &);

    template <typename FilterFunc, typename Container>
    class Filter {
        // The filter function is the only thing allowed to create a Filter
        friend Filter filter<FilterFunc, Container>(FilterFunc, Container &);

        
        protected:
            // Type of the Container::Iterator, but since the name of that 
            // iterator can be anything, we have to grab it with this
            using contained_iter_type =
                decltype(std::declval<Container>().begin());

            // The type returned when dereferencing the Container::Iterator
            using contained_iter_ret =
                decltype(std::declval<contained_iter_type>().operator*());

            Container & container;
            FilterFunc filter_func;
            
            // Value constructor for use only in the filter function
            Filter(FilterFunc filter_func, Container & container) :
                container(container),
                filter_func(filter_func)
            { }
            Filter () = delete;
            Filter & operator=(const Filter &) = delete;
            // Default copy constructor used

        public:
            class Iterator {
                protected:
                    contained_iter_type sub_iter;
                    const contained_iter_type sub_end;
                    FilterFunc filter_func;

                    // increment until the iterator points to is true on the 
                    // predicate.  Called by constructor and operator++
                    void skip_failures() { 
                        while (this->sub_iter != this->sub_end
                                && !this->filter_func(*this->sub_iter)) {
                            ++this->sub_iter;
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
                        this->skip_failures();
                    } 

                    contained_iter_ret operator*() const {
                        return *this->sub_iter;
                    }

                    Iterator & operator++() { 
                        ++this->sub_iter;
                        this->skip_failures();
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

    // Helper function to instantiate a FilterDefault
    template <typename FilterFunc, typename Container>
    Filter<FilterFunc, Container> filter(
            FilterFunc filter_func, Container & container) {
        return Filter<FilterFunc, Container>(filter_func, container);
    }

    //Forward declarations of FilterDefault and filter
    template <typename Container>
    class FilterDefault;

    template <typename Container>
    FilterDefault<Container> filter(Container &);

    template <typename Container>
    class FilterDefault {
        // The filter function is the only thing allowed to create a FilterDefault
        friend FilterDefault filter<Container>(Container &);

        
        protected:
            // Type of the Container::Iterator, but since the name of that 
            // iterator can be anything, we have to grab it with this
            using contained_iter_type =
                decltype(std::declval<Container>().begin());

            // The type returned when dereferencing the Container::Iterator
            using contained_iter_ret =
                decltype(std::declval<contained_iter_type>().operator*());

            Container & container;
            
            // Value constructor for use only in the filter function
            FilterDefault(Container & container) :
                container(container)
            { }
            FilterDefault () = delete;
            FilterDefault & operator=(const FilterDefault &) = delete;
            // Default copy constructor used

        public:
            class Iterator {
                protected:
                    contained_iter_type sub_iter;
                    const contained_iter_type sub_end;

                    // increment until the iterator points to is true on the 
                    // predicate.  Called by constructor and operator++
                    void skip_failures() { 
                        while (this->sub_iter != this->sub_end
                                && !(*this->sub_iter)) {
                            ++this->sub_iter;
                        }
                    }

                public:
                    Iterator (contained_iter_type iter,
                            contained_iter_type end) :
                        sub_iter(iter),
                        sub_end(end)
                    { 
                        this->skip_failures();
                    } 

                    contained_iter_ret operator*() const {
                        return *this->sub_iter;
                    }

                    Iterator & operator++() { 
                        ++this->sub_iter;
                        this->skip_failures();
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

    // Helper function to instantiate a FilterDefault
    template <typename Container>
    FilterDefault<Container> filter(Container & container) {
        return FilterDefault<Container>(container);
    }
}

#endif //ifndef FILTER__H__
