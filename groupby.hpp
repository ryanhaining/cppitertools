#ifndef GROUP__BY__HPP
#define GROUP__BY__HPP

#include <iterbase.hpp>

#include <utility>
#include <iterator>

namespace iter {

    template <typename Container, typename KeyFunc>
    class GroupBy;

    template <typename Container, typename KeyFunc>
    GroupBy<Container, KeyFunc> groupby(Container &, KeyFunc);

    template <typename Container, typename KeyFunc>
    class GroupBy : IterBase<Container> {
        private:
            Container & container;
            KeyFunc key_func;

            // The filter function is the only thing allowed to create a Filter
            friend GroupBy groupby<Container, KeyFunc>(Container &, KeyFunc);

            using typename IterBase<Container>::contained_iter_type;

            using typename IterBase<Container>::contained_iter_ret;

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
                    friend class GroupIterator;
                    Iterator *owner;
                    key_func_ret key;
                    mutable bool completed = false;

                    Group(Iterator *owner, key_func_ret key) :
                        owner(owner),
                        key(key)
                    { }

                public:
                    ~Group() {
                        if (!this->completed) {
                            for (auto iter = this->begin(), end = this->end();
                                    iter != end;
                                    ++iter) { }
                        }
                    }
                           


                    Group () = delete;
                    Group (const Group &other) :
                            owner(other.owner),
                            key(other.key),
                            completed(other.completed) {
                        other.completed = true;
                    }

                    class GroupIterator {
                        private:
                            Iterator * owner;
                            const key_func_ret key;
                            const Group * group;

                            bool not_at_end() const {
                                return !this->owner->exhausted() &&
                                    this->owner->next_key() == this->key;
                            }

                        public:
                            GroupIterator(Iterator * owner, const Group *group,
                                          key_func_ret key) :
                                owner(owner),
                                key(key),
                                group(group)
                            { }

                            GroupIterator(const GroupIterator &) = default;

                            bool operator!=(const GroupIterator &) const {
                                if (this->not_at_end()) {
                                    return true;
                                } else {
                                    this->group->completed = true;
                                    return false;
                                }
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
                        return GroupIterator(this->owner, this, key);
                    }

                    GroupIterator end() const {
                        return GroupIterator(this->owner, this, key);
                    }

            };


            Iterator begin() const {
                return Iterator(
                        std::begin(this->container),
                        std::end(this->container),
                        this->key_func);
            }

            Iterator end() const {
                return Iterator(
                        std::end(this->container),
                        std::end(this->container),
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
                typename IterBase<Container>::contained_iter_ret;
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
