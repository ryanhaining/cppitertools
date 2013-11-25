#ifndef COMPRESS__H__
#define COMPRESS__H__

#include <iterbase.hpp>

#include <utility>

namespace iter {

    //Forward declarations of Compressed and compress
    template <typename Container, typename Selector>
    class Compressed;

    template <typename Container, typename Selector>
    Compressed<Container, Selector> compress(Container &&, Selector &&);

    template <typename T, typename Selector>
    Compressed<std::initializer_list<T>, Selector> compress(
            std::initializer_list<T> &&, Selector &&);


    template <typename Container, typename Selector>
    class Compressed : public IterBase<Container> {
        private:
            Container & container;
            Selector & selectors;

            // The only thing allowed to directly instantiate an Compressed is
            // the compress function
            friend Compressed compress<Container, Selector>(
                    Container &&, Selector &&);
            template <typename T, typename Sel>
            friend Compressed<std::initializer_list<T>, Sel> compress(
                    std::initializer_list<T> &&, Sel &&);

            using typename IterBase<Container>::contained_iter_type;

            using typename IterBase<Container>::contained_iter_ret;

            // Selector::Iterator type
            using selector_iter_type = decltype(std::begin(selectors));
            
            // Value constructor for use only in the compress function
            Compressed(Container && container, Selector && selectors) :
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
                        std::begin(this->container), std::end(this->container),
                        std::begin(this->selectors), std::end(this->selectors));
            }

            Iterator end() const {
                return Iterator(
                        std::end(this->container), std::end(this->container),
                        std::end(this->selectors), std::end(this->selectors));
            }

    };

    // Helper function to instantiate an Compressed
    template <typename Container, typename Selector>
    Compressed<Container, Selector> compress(
            Container && container, Selector && selectors) {
        return Compressed<Container, Selector>(
                std::forward<Container>(container),
                std::forward<Selector>(selectors));
    }

    template <typename T, typename Selector>
    Compressed<std::initializer_list<T>, Selector> compress(
            std::initializer_list<T> && il, Selector && selectors) {
        return Compressed<std::initializer_list<T>, Selector>(
                std::move(il),
                std::forward<Selector>(selectors));
    }
}

#endif //ifndef COMPRESS__H__
