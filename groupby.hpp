#ifndef ITER_GROUP_BY_HPP_
#define ITER_GROUP_BY_HPP_

// this is easily the most functionally complex itertool

#include "internal/iterator_wrapper.hpp"
#include "internal/iterbase.hpp"

#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

namespace iter {
  namespace impl {
    template <typename Container, typename KeyFunc>
    class GroupProducer;

    struct Identity {
      template <typename T>
      const T& operator()(const T& t) const {
        return t;
      }
    };

    using GroupByFn = IterToolFnOptionalBindSecond<GroupProducer, Identity>;
  }
  constexpr impl::GroupByFn groupby{};
}

template <typename Container, typename KeyFunc>
class iter::impl::GroupProducer {
 private:
  Container container;
  KeyFunc key_func;

  friend GroupByFn;

  using key_func_ret = std::result_of_t<KeyFunc(iterator_deref<Container>)>;

  GroupProducer(Container&& in_container, KeyFunc in_key_func)
      : container(std::forward<Container>(in_container)),
        key_func(in_key_func) {}

 public:
  GroupProducer(GroupProducer&&) = default;

  class Iterator;
  class Group;

 private:
  using KeyGroupPair = std::pair<key_func_ret, Group>;
  using Holder = DerefHolder<iterator_deref<Container>>;

 public:
  class Iterator : public std::iterator<std::input_iterator_tag, KeyGroupPair> {
   private:
    IteratorWrapper<Container> sub_iter;
    IteratorWrapper<Container> sub_end;
    Holder item;
    KeyFunc* key_func;

    std::unique_ptr<KeyGroupPair> current_key_group_pair;

   public:
    Iterator(IteratorWrapper<Container>&& si, IteratorWrapper<Container>&& end,
        KeyFunc& in_key_func)
        : sub_iter{std::move(si)},
          sub_end{std::move(end)},
          key_func(&in_key_func) {
      if (this->sub_iter != this->sub_end) {
        this->item.reset(*this->sub_iter);
      }
    }

    Iterator(const Iterator& other)
        : sub_iter{other.sub_iter},
          sub_end{other.sub_end},
          item{other.item},
          key_func{other.key_func} {}

    Iterator& operator=(const Iterator& other) {
      if (this == &other) {
        return *this;
      }
      this->sub_iter = other.sub_iter;
      this->sub_end = other.sub_end;
      this->item = other.item;
      this->key_func = other.key_func;
      this->current_key_group_pair.reset();
      return *this;
    }

    ~Iterator() = default;

    // NOTE the implicitly generated move constructor would
    // be wrong

    KeyGroupPair& operator*() {
      set_key_group_pair();
      return *this->current_key_group_pair;
    }

    KeyGroupPair* operator->() {
      set_key_group_pair();
      return this->current_key_group_pair.get();
    }

    Iterator& operator++() {
      if (!this->current_key_group_pair) {
        this->set_key_group_pair();
      }
      this->current_key_group_pair.reset();
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    bool operator!=(const Iterator& other) const {
      return this->sub_iter != other.sub_iter;
    }

    bool operator==(const Iterator& other) const {
      return !(*this != other);
    }

    void increment_iterator() {
      if (this->sub_iter != this->sub_end) {
        ++this->sub_iter;
        if (this->sub_iter != this->sub_end) {
          this->item.reset(*this->sub_iter);
        }
      }
    }

    bool exhausted() const {
      return !(this->sub_iter != this->sub_end);
    }

    typename Holder::reference get() {
      return this->item.get();
    }

    typename Holder::pointer get_ptr() {
      return this->item.get_ptr();
    }

    key_func_ret next_key() {
      return (*this->key_func)(this->item.get());
    }

    void set_key_group_pair() {
      if (!this->current_key_group_pair) {
        this->current_key_group_pair =
            std::make_unique<KeyGroupPair>((*this->key_func)(this->item.get()),
                Group{*this, this->next_key()});
      }
    }
  };

  class Group {
   private:
    friend Iterator;
    friend class GroupIterator;
    Iterator& owner;
    key_func_ret key;

    // completed is set if a Group is iterated through
    // completely.  It is checked in the destructor, and
    // if the Group has not been completed, the destructor
    // exhausts it.  This ensures that the next Group starts
    // at the correct position when the user short-circuits
    // iteration over a Group.
    // The move constructor sets the rvalue's completed
    // attribute to true, so its destructor doesn't do anything
    // when called.
    bool completed = false;

    Group(Iterator& in_owner, key_func_ret in_key)
        : owner(in_owner), key(in_key) {}

   public:
    ~Group() {
      if (!this->completed) {
        for (auto iter = this->begin(), end = this->end(); iter != end;
             ++iter) {
        }
      }
    }

    // move-constructible, non-copy-constructible, non-assignable
    Group(Group&& other) noexcept
        : owner(other.owner), key{other.key}, completed{other.completed} {
      other.completed = true;
    }

    class GroupIterator : public std::iterator<std::input_iterator_tag,
                              iterator_traits_deref<Container>> {
     private:
      std::remove_reference_t<key_func_ret>* key;
      Group* group_p;

      bool not_at_end() {
        return !this->group_p->owner.exhausted()
               && this->group_p->owner.next_key() == *this->key;
      }

     public:
      GroupIterator(Group* in_group_p, key_func_ret& in_key)
          : key{&in_key}, group_p{in_group_p} {}

      bool operator!=(const GroupIterator& other) const {
        return !(*this == other);
      }

      bool operator==(const GroupIterator& other) const {
        return this->group_p == other.group_p;
      }

      GroupIterator& operator++() {
        this->group_p->owner.increment_iterator();
        if (!this->not_at_end()) {
          this->group_p->completed = true;
          this->group_p = nullptr;
        }
        return *this;
      }

      GroupIterator operator++(int) {
        auto ret = *this;
        ++*this;
        return ret;
      }

      iterator_deref<Container> operator*() {
        return this->group_p->owner.get();
      }

      typename Holder::pointer operator->() {
        return this->group_p->owner.get_ptr();
      }
    };

    GroupIterator begin() {
      return {this, key};
    }

    GroupIterator end() {
      return {nullptr, key};
    }
  };

  Iterator begin() {
    return {
        std::begin(this->container), std::end(this->container), this->key_func};
  }

  Iterator end() {
    return {
        std::end(this->container), std::end(this->container), this->key_func};
  }
};

#endif
