#ifndef IMAP__H__
#define IMAP__H__

#include <utility>
#include <type_traits>

namespace iter {

    //Forward declarations of IMap and imap
    template <typename MapFunc, typename Container>
    class IMap;

    template <typename MapFunc, typename Container>
    IMap<MapFunc, Container> imap(MapFunc, Container &);

    template <typename MapFunc, typename Container>
    class IMap {
        // The imap function is the only thing allowed to create a IMap
        friend IMap imap<MapFunc, Container>(MapFunc, Container &);

        // Type of the Container::Iterator, but since the name of that 
        // iterator can be anything, we have to grab it with this
        using contained_iter_type =
            decltype(std::declval<Container>().begin());

        // The type returned when dereferencing the Container::Iterator
        using contained_iter_ret =
            decltype(std::declval<contained_iter_type>().operator*());

        private:
            Container & container;
            MapFunc map_func;
            
            // Value constructor for use only in the imap function
            IMap(MapFunc map_func, Container & container) :
                container(container),
                map_func(map_func)
            { }
            IMap () = delete;
            IMap & operator=(const IMap &) = delete;

        public:
            IMap (const IMap &) = default;

            class Iterator {
                private:
                    contained_iter_type sub_iter;
                    const contained_iter_type sub_end;
                    MapFunc map_func;

                public:
                    Iterator (contained_iter_type iter,
                            contained_iter_type end,
                            MapFunc map_func) :
                        sub_iter(iter),
                        sub_end(end),
                        map_func(map_func)
                    { } 

                    auto operator*() const -> decltype(map_func(*this->sub_iter)) {
                        return map_func(*this->sub_iter);
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
                        this->map_func);
            }

            Iterator end() const {
                return Iterator(
                        this->container.end(),
                        this->container.end(),
                        this->map_func);
            }

    };

    // Helper function to instantiate a IMap
    template <typename MapFunc, typename Container>
    IMap<MapFunc, Container> imap(
            MapFunc map_func, Container & container) {
        return IMap<MapFunc, Container>(map_func, container);
    }

}

#endif //ifndef IMAP__H__
