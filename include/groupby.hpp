#ifndef ITER_GROUP_BY_HPP_
#define ITER_GROUP_BY_HPP_

// this is easily the most functionally complex itertool

#include "internal/iterator_wrapper.hpp"
#include "internal/iterbase.hpp"

#include <functional>
#include <iterator>
#include <memory>
#include <optional>
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
  Container container_;
  mutable KeyFunc key_func_;

  friend GroupByFn;

  template <typename T>
  using key_func_ret = std::invoke_result_t<KeyFunc, iterator_deref<T>>;

  GroupProducer(Container&& container, KeyFunc key_func)
      : container_(std::forward<Container>(container)), key_func_(key_func) {}

 public:
  GroupProducer(GroupProducer&&) = default;

  template <typename T>
  class Iterator;
  template <typename T>
  class Group;

 private:
  template <typename T>
  using KeyGroupPair = std::pair<key_func_ret<T>, Group<T>>;
  template <typename T>
  using Holder = DerefHolder<iterator_deref<T>>;

 public:
  template <typename ContainerT>
  class Iterator {
   private:
    template <typename>
    friend class Iterator;
    IteratorWrapper<ContainerT> sub_iter_;
    IteratorWrapper<ContainerT> sub_end_;
    Holder<ContainerT> item_;
    KeyFunc* key_func_;
    std::optional<KeyGroupPair<ContainerT>> current_key_group_pair_;

   public:
    using iterator_category = std::input_iterator_tag;
    using value_type = KeyGroupPair<ContainerT>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    Iterator(IteratorWrapper<ContainerT>&& sub_iter,
        IteratorWrapper<ContainerT>&& sub_end, KeyFunc& key_func)
        : sub_iter_{std::move(sub_iter)},
          sub_end_{std::move(sub_end)},
          key_func_(&key_func) {
      if (sub_iter_ != sub_end_) {
        item_.reset(*sub_iter_);
      }
    }

    Iterator(const Iterator& other)
        : sub_iter_{other.sub_iter_},
          sub_end_{other.sub_end_},
          item_{other.item_},
          key_func_{other.key_func_} {}

    Iterator& operator=(const Iterator& other) {
      if (this == &other) {
        return *this;
      }
      sub_iter_ = other.sub_iter_;
      sub_end_ = other.sub_end_;
      item_ = other.item_;
      key_func_ = other.key_func_;
      current_key_group_pair_.reset();
      return *this;
    }

    ~Iterator() = default;

    // NOTE the implicitly generated move constructor would
    // be wrong

    KeyGroupPair<ContainerT>& operator*() {
      set_key_group_pair();
      return *current_key_group_pair_;
    }

    KeyGroupPair<ContainerT>* operator->() {
      set_key_group_pair();
      return &*current_key_group_pair_;
    }

    Iterator& operator++() {
      if (!current_key_group_pair_) {
        set_key_group_pair();
      }
      current_key_group_pair_.reset();
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    template <typename T>
    bool operator!=(const Iterator<T>& other) const {
      return sub_iter_ != other.sub_iter_;
    }

    template <typename T>
    bool operator==(const Iterator<T>& other) const {
      return !(*this != other);
    }

    void increment_iterator() {
      if (sub_iter_ != sub_end_) {
        ++sub_iter_;
        if (sub_iter_ != sub_end_) {
          item_.reset(*sub_iter_);
        }
      }
    }

    bool exhausted() const {
      return !(sub_iter_ != sub_end_);
    }

    typename Holder<ContainerT>::reference get() {
      return item_.get();
    }

    typename Holder<ContainerT>::pointer get_ptr() {
      return item_.get_ptr();
    }

    key_func_ret<ContainerT> next_key() {
      return std::invoke(*key_func_, item_.get());
    }

    void set_key_group_pair() {
      if (!current_key_group_pair_) {
        current_key_group_pair_.emplace(std::invoke(*key_func_, item_.get()),
            Group<ContainerT>{*this, next_key()});
      }
    }
  };

  template <typename ContainerT>
  class Group {
   private:
    template <typename>
    friend class Iterator;
    friend class GroupIterator;
    Iterator<ContainerT>& owner_;
    key_func_ret<ContainerT> key_;

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

    Group(Iterator<ContainerT>& owner, key_func_ret<ContainerT> key)
        : owner_(owner), key_(key) {}

   public:
    ~Group() {
      if (!completed) {
        for (auto iter = begin(), end_it = end(); iter != end_it; ++iter) {
        }
      }
    }

    // move-constructible, non-copy-constructible, non-assignable
    Group(Group&& other) noexcept
        : owner_(other.owner_), key_{other.key_}, completed{other.completed} {
      other.completed = true;
    }

    class GroupIterator {
     private:
      std::remove_reference_t<key_func_ret<ContainerT>>* key_;
      Group* group_p_;

      bool not_at_end() {
        return !group_p_->owner_.exhausted()
               && group_p_->owner_.next_key() == *key_;
      }

     public:
      using iterator_category = std::input_iterator_tag;
      using value_type = iterator_traits_deref<ContainerT>;
      using difference_type = std::ptrdiff_t;
      using pointer = value_type*;
      using reference = value_type&;

      // TODO template this? idk if it's relevant here
      GroupIterator(Group* group_p, key_func_ret<ContainerT>& key)
          : key_{&key}, group_p_{group_p} {}

      bool operator!=(const GroupIterator& other) const {
        return !(*this == other);
      }

      bool operator==(const GroupIterator& other) const {
        return group_p_ == other.group_p_;
      }

      GroupIterator& operator++() {
        group_p_->owner_.increment_iterator();
        if (!not_at_end()) {
          group_p_->completed = true;
          group_p_ = nullptr;
        }
        return *this;
      }

      GroupIterator operator++(int) {
        auto ret = *this;
        ++*this;
        return ret;
      }

      iterator_deref<ContainerT> operator*() {
        return group_p_->owner_.get();
      }

      typename Holder<ContainerT>::pointer operator->() {
        return group_p_->owner_.get_ptr();
      }
    };

    GroupIterator begin() {
      return {this, key_};
    }

    GroupIterator end() {
      return {nullptr, key_};
    }
  };

  Iterator<Container> begin() {
    return {get_begin(container_), get_end(container_), key_func_};
  }

  Iterator<Container> end() {
    return {get_end(container_), get_end(container_), key_func_};
  }

  Iterator<AsConst<Container>> begin() const {
    return {get_begin(std::as_const(container_)),
        get_end(std::as_const(container_)), key_func_};
  }

  Iterator<AsConst<Container>> end() const {
    return {get_end(std::as_const(container_)),
        get_end(std::as_const(container_)), key_func_};
  }
};

#endif
