#ifndef DROPWHILE__H__
#define DROPWHILE__H__

#include <utility>

namespace iter {

    //Forward declarations of DropWhile and dropwhile
    template <typename FilterFunc, typename Container>
    class DropWhile;

    template <typename FilterFunc, typename Container>
    DropWhile<FilterFunc, Container> dropwhile(FilterFunc, Container &);

    template <typename FilterFunc, typename Container>
    class DropWhile {
        private:
            Container & container;
            FilterFunc filter_func;

            friend DropWhile dropwhile<FilterFunc, Container>(
                    FilterFunc, Container &);

            // Type of the Container::Iterator, but since the name of that 
            // iterator can be anything, we have to grab it with this
            using contained_iter_type = decltype(container.begin());

            // The type returned when dereferencing the Container::Iterator
            using contained_iter_ret = decltype(container.begin().operator*());

            
            // Value constructor for use only in the dropwhile function
            DropWhile(FilterFunc filter_func, Container & container) :
                container(container),
                filter_func(filter_func)
            { }
            DropWhile () = delete;
            DropWhile & operator=(const DropWhile &) = delete;

        public:
            DropWhile(const Dropwhile &) = default;
            class Iterator {
                private:
                    contained_iter_type sub_iter;
                    const contained_iter_type sub_end;
                    FilterFunc filter_func;

                    // skip all values for which the predicate is true
                    void skip_passes() { 
                        while (this->sub_iter != this->sub_end
                                && this->filter_func(*this->sub_iter)) {
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
                        this->skip_passes();
                    } 

                    contained_iter_ret operator*() const {
                        return *this->sub_iter;
                    }

                    Iterator & operator++() { 
                        ++this->sub_iter;
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

    // Helper function to instantiate a DropWhile
    template <typename FilterFunc, typename Container>
    DropWhile<FilterFunc, Container> dropwhile(
            FilterFunc filter_func, Container & container) {
        return DropWhile<FilterFunc, Container>(filter_func, container);
    }

}

#endif //ifndef DROPWHILE__H__
