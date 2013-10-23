#ifndef GROUP__BY__HPP
#define GROUP__BY__HPP


#include <utility>


namespace iter {

    template <typename Container, typename KeyFunc>
    class GroupBy;

    template <typename Container, typename KeyFunc>
    GroupBy<Container, KeyFunc> groupby(Container &, KeyFunc);

    template <typename Container, typename KeyFunc>
    class GroupBy {
        private:
            Container & container;
            KeyFunc key_func;

            // The filter function is the only thing allowed to create a Filter
            friend GroupBy groupby<Container, KeyFunc>(Container &, KeyFunc);

            // Type of the Container::Iterator, but since the name of that 
            // iterator can be anything, we have to grab it with this
            using contained_iter_type = decltype(container.begin());

            // The type returned when dereferencing the Container::Iterator
            using contained_iter_ret = decltype(container.begin().operator*());

            using key_func_ret =
                decltype(std::declval<KeyFunc>()(
                            std::declval<contained_iter_ret>()));

            GroupBy(Container & container, KeyFunc key_func) :
                container(container),
                key_func(key_func)
            { }
            GroupBy () = delete;
            GroupBy & operator=(const GroupBy &) = delete;

        public:
            GroupBy(const GroupBy &) = default;

            class Iterator;
            class Group;

            class Iterator {
                private:
                    contained_iter_type sub_iter;
                    contained_iter_type sub_iter_peek;
                    const contained_iter_type sub_end;
                    KeyFunc key_func;

                    using KeyGroupPair =
                        std::pair<key_func_ret, Group>;

                public:
                    Iterator (contained_iter_type si,
                              contained_iter_type end,
                              KeyFunc key_func) :
                        sub_iter(si),
                        sub_end(end),
                        key_func(key_func)
                    { }

                    KeyGroupPair operator*() {
                        return KeyGroupPair(
                                this->key_func(*this->sub_iter),
                                Group(
                                    this, 
                                    this->key_func(*this->sub_iter)));
                    }

                    Iterator & operator++() { 
                        return *this;
                    }

                    bool operator!=(const Iterator &) const {
                        return !this->exhausted();
                    }

                    void increment_iterator() {
                        if (this->sub_iter != this->sub_end) {
                            ++this->sub_iter;
                        }
                    }

                    bool exhausted() const {
                        return this->sub_iter == this->sub_end;
                    }

                    contained_iter_ret current() const {
                        return *this->sub_iter;
                    }

                    key_func_ret next_key() const {
                        return this->key_func(*this->sub_iter);
                    }
            };


            class Group {
                private:
                    friend Iterator;
                    Iterator *owner;
                    key_func_ret key;

                    Group(Iterator *owner, key_func_ret key) :
                        owner(owner),
                        key(key)
                    { }


                    Group () = delete;
                public:
                    Group (const Group &) = default;

                    class GroupIterator {
                        private:
                            Iterator * owner;
                            const key_func_ret key;

                        public:
                            GroupIterator(Iterator * owner, key_func_ret key) :
                                owner(owner),
                                key(key)
                            { }

                            GroupIterator(const GroupIterator &) = default;

                            bool operator!=(const GroupIterator &) const {
                                return !this->owner->exhausted() &&
                                    this->owner->next_key() == this->key;
                            }

                            GroupIterator & operator++() {
                                this->owner->increment_iterator();
                                return *this;
                            }

                            contained_iter_ret operator*() const {
                                return this->owner->current();
                            }
                    };

                    GroupIterator begin() const {
                        return GroupIterator(this->owner, key);
                    }

                    GroupIterator end() const {
                        return GroupIterator(this->owner, key);
                    }

            };


            Iterator begin() const {
                return Iterator(
                        this->container.begin(),
                        this->container.end(),
                        this->key_func);
            }

            Iterator end() const {
                return Iterator(
                        this->container.end(),
                        this->container.end(),
                        this->key_func);
            }

    };

    template <typename Container, typename KeyFunc>
    GroupBy<Container, KeyFunc> groupby(
            Container & container, KeyFunc key_func) {
        return GroupBy<Container, KeyFunc>(container, key_func);
    }

    template <typename Container>
    class ItemReturner {
        private:
            using contained_iter_ret =
                decltype(std::declval<Container>().begin().operator*());
        public:
            ItemReturner() = default;
            contained_iter_ret operator() (contained_iter_ret item) const {
                return item;
            }
    };

    template <typename Container>
    auto groupby(Container & container) ->
            decltype(groupby(container, ItemReturner<Container>())) {
        return groupby(container, ItemReturner<Container>());
    }

}


#endif //#ifndef GROUP__BY__HPP
