#ifndef CYCLE__H__
#define CYCLE__H__

namespace iter {

    //Forward declarations of Cycle and cycle
    template <typename Container>
    class Cycle;

    template <typename Container>
    Cycle<Container> cycle(Container &);


    template <typename Container>
    class Cycle {
        // The cycle function is the only thing allowed to create a Cycle
        friend Cycle cycle<Container>(Container &);

        // Type of the Container::Iterator, but since the name of that 
        // iterator can be anything, we have to grab it with this
        using contained_iter_type =
            decltype(((Container *)nullptr)->begin());

        // The type returned when dereferencing the Container::Iterator
        using contained_iter_ret =
            decltype(((contained_iter_type *)nullptr)->operator*());

        private:
            Container & container;
            
            // Value constructor for use only in the cycle function
            Cycle(Container & container) : container(container) { }
            Cycle () = delete;
            Cycle & operator=(const Cycle &) = delete;
            // Default copy constructor used

        public:
            class Iterator {
                private:
                    contained_iter_type sub_iter;
                    const contained_iter_type begin;
                    const contained_iter_type end;
                public:
                    Iterator (contained_iter_type iter,
                            contained_iter_type end) :
                        sub_iter(iter),
                        begin(iter),
                        end(end)
                    { } 

                    contained_iter_ret operator*() const {
                        return *this->sub_iter;
                    }

                    Iterator & operator++() { 
                        ++this->sub_iter;
                        // reset to beginning upon reaching the end
                        if (this->sub_iter == this->end) {
                            this->sub_iter = this->begin;
                        }
                        return *this;
                    }

                    constexpr bool operator!=(const Iterator &) const {
                        return true;
                    }
            };

            Iterator begin() const {
                return Iterator(this->container.begin(),
                        this->container.end());
            }

            Iterator end() const {
                return Iterator(this->container.end(), this->container.end());
            }

    };

    // Helper function to instantiate an Filter
    template <typename Container>
    Cycle<Container> cycle(Container & container) {
        return Cycle<Container>(container);
    }

}

#endif //ifndef CYCLE__H__
