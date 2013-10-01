#ifndef COMPRESS__H__
#define COMPRESS__H__

#include <utility>

// TODO everything in here

namespace iter {

    //Forward declarations of Compressed and compress
    template <typename Container, typename Selector>
    class Compressed;

    template <typename Container, typename Selector>
    Compressed<Container, Selector> compress(Container &, Selector &);


    template <typename Container, typename Selector>
    class Compressed {
        // The only thing allowed to directly instantiate an Compressed is
        // the compress function
        friend Compressed compress<Container, Selector>(
                Container &, Selector &);

        // Type of the Container::Iterator, but since the name of that 
        // iterator can be anything, we have to grab it with this
        using contained_iter_type =
            decltype(std::declval<Container>().begin());

        // The type returned when dereferencing the Container::Iterator
        using contained_iter_ret =
            decltype(std::declval<contained_iter_type>().operator*());

        // Selector::Iterator type
        using selector_iter_type =
            decltype(std::declval<Selector>().begin());

        private:
            Container & container;
            Selector & selectors;
            
            // Value constructor for use only in the compress function
            Compressed(Container & container, Selector & selectors) :
                container(container),
                selectors(selectors)
            { }
            Compressed () = delete;
            Compressed & operator=(const Compressed &) = delete;

        public:
            Compressed (const Compressed &) = default;

            class Iterator {
                private:
                    contained_iter_type sub_iter;
                    const contained_iter_type sub_end;

                    selector_iter_type selector_iter;
                    const selector_iter_type selector_end;

                    void increment_iterators() {
                        ++this->sub_iter;
                        ++this->selector_iter;
                    }

                    void skip_failures() {
                        while (this->sub_iter != this->sub_end &&
                                !*this->selector_iter) {
                            this->increment_iterators();
                        }
                    }

                public:
                    Iterator (contained_iter_type cont_iter,
                              contained_iter_type cont_end,
                              selector_iter_type sel_iter,
                              selector_iter_type sel_end) :
                        sub_iter(cont_iter),
                        sub_end(cont_end),
                        selector_iter(sel_iter),
                        selector_end(sel_end)
                    { 
                        this->skip_failures();
                    } 

                    contained_iter_ret operator*() const {
                        return *this->sub_iter;
                    }

                    Iterator & operator++() { 
                        this->increment_iterators();
                        this->skip_failures();
                        return *this;
                    }

                    bool operator!=(const Iterator & other) const {
                        return this->sub_iter != other.sub_iter &&
                            this->selector_iter != other.selector_iter;
                    }
            };

            Iterator begin() const {
                return Iterator(
                        this->container.begin(), this->container.end(),
                        this->selectors.begin(), this->selectors.end());
            }

            Iterator end() const {
                return Iterator(
                        this->container.end(), this->container.end(),
                        this->selectors.end(), this->selectors.end());
            }

    };

    // Helper function to instantiate an Compressed
    template <typename Container, typename Selector>
    Compressed<Container, Selector> compress(
            Container & container, Selector & selectors) {
        return Compressed<Container, Selector>(container, selectors);
    }

}

#endif //ifndef COMPRESS__H__
