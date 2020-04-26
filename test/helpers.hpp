#ifndef TEST_HELPER_H_
#define TEST_HELPER_H_

#include <internal/iterbase.hpp>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace itertest {

  namespace archetypes {
    template <typename Iter>
    class InputIterator {
      using Traits = std::iterator_traits<Iter>;
      Iter it_;

     public:
      using iterator_category = std::input_iterator_tag;
      using value_type = typename Traits::value_type;
      using difference_type = typename Traits::difference_type;
      using pointer = Iter;
      using reference = typename Traits::reference;

      Iter base() const {
        return it_;
      }

      explicit InputIterator(Iter it) : it_(it) {}

      reference operator*() const {
        return *it_;
      }
      pointer operator->() const {
        return it_;
      }

      InputIterator& operator++() {
        ++it_;
        return *this;
      }
      InputIterator operator++(int) {
        InputIterator tmp(*this);
        ++(*this);
        return tmp;
      }

      friend bool operator==(
          const InputIterator& LHS, const InputIterator& RHS) {
        return LHS.it_ == RHS.it_;
      }
      friend bool operator!=(
          const InputIterator& LHS, const InputIterator& RHS) {
        return LHS.it_ != RHS.it_;
      }
    };

    template <typename Iter>
    class ForwardIterator {
      using Traits = std::iterator_traits<Iter>;
      Iter it_;

     public:
      using iterator_category = std::forward_iterator_tag;
      using value_type = typename Traits::value_type;
      using difference_type = typename Traits::difference_type;
      using pointer = Iter;
      using reference = typename Traits::reference;

      Iter base() const {
        return it_;
      }

      ForwardIterator() : it_() {}
      explicit ForwardIterator(Iter it) : it_(it) {}

      reference operator*() const {
        return *it_;
      }
      pointer operator->() const {
        return it_;
      }

      ForwardIterator& operator++() {
        ++it_;
        return *this;
      }
      ForwardIterator operator++(int) {
        ForwardIterator tmp(*this);
        ++(*this);
        return tmp;
      }

      friend bool operator==(
          const ForwardIterator& LHS, const ForwardIterator& RHS) {
        return LHS.it_ == RHS.it_;
      }
      friend bool operator!=(
          const ForwardIterator& LHS, const ForwardIterator& RHS) {
        return LHS.it_ != RHS.it_;
      }
    };

    template <typename ContainerT, template <typename Iter> typename IterWrap>
    class Container {
      ContainerT container_;

     public:
      using iterator = IterWrap<typename ContainerT::iterator>;
      using const_iterator = IterWrap<typename ContainerT::const_iterator>;

      Container(ContainerT const& c) : container_(c) {}

      iterator begin() {
        return iterator(container_.begin());
      }
      iterator end() {
        return iterator(container_.end());
      }
      const_iterator begin() const {
        return const_iterator(container_.begin());
      }
      const_iterator end() const {
        return const_iterator(container_.end());
      }
    };
  }  // namespace archetypes

  // non-copyable. non-movable. non-default-constructible
  class SolidInt {
   private:
    const int i;

   public:
    constexpr SolidInt(int n) : i{n} {}

    constexpr int getint() const {
      return this->i;
    }

    SolidInt() = delete;
    SolidInt(const SolidInt&) = delete;
    SolidInt& operator=(const SolidInt&) = delete;
    SolidInt& operator=(SolidInt&&) = delete;
    SolidInt(SolidInt&&) = delete;
  };

  class Integer {
   private:
    int i_{};

   public:
    constexpr Integer(int i) : i_{i} {}
    constexpr bool is_zero() const {
      return i_ == 0;
    }
    constexpr bool operator==(const Integer& other) const {
      return i_ == other.i_;
    }
    constexpr bool operator!=(const Integer& other) const {
      return i_ != other.i_;
    }

    constexpr bool is_positive() const {
      return i_ > 0;
    }
  };

  namespace {
    struct DoubleDereferenceError : std::exception {
      const char* what() const noexcept override {
        return "Iterator dereferenced twice without increment";
      }
    };

    // this class's iterator will throw if it's dereference twice without
    // an increment in between
    class InputIterable {
     public:
      class Iterator {
       private:
        int i;
        bool was_incremented = true;

       public:
        Iterator(int n) : i{n} {}

        Iterator& operator++() {
          ++this->i;
          this->was_incremented = true;
          return *this;
        }

        int operator*() {
          if (!this->was_incremented) {
            throw DoubleDereferenceError{};
          }
          this->was_incremented = false;
          return this->i;
        }

        bool operator!=(const Iterator& other) const {
          return this->i != other.i;
        }
      };

      Iterator begin() {
        return {0};
      }

      Iterator end() {
        return {5};
      }
    };
  }

  // BasicIterable provides a minimal forward iterator
  // operator++(), operator!=(const BasicIterable&), operator*()
  // move constructible only
  // not copy constructible, move assignable, or copy assignable
  template <typename T>
  class BasicIterable {
   private:
    T* data;
    std::size_t size;
    bool was_moved_from_ = false;
    mutable bool was_copied_from_ = false;

   public:
    BasicIterable(std::initializer_list<T> il)
        : data{new T[il.size()]}, size{il.size()} {
      // would like to use enumerate, can't because it's for unit
      // testing enumerate
      std::size_t i = 0;
      for (auto&& e : il) {
        data[i] = e;
        ++i;
      }
    }

    BasicIterable& operator=(BasicIterable&&) = delete;
    BasicIterable& operator=(const BasicIterable&) = delete;

#ifndef DEFINE_BASIC_ITERABLE_COPY_CTOR
    BasicIterable(const BasicIterable&) = delete;
#else
    BasicIterable(const BasicIterable& other)
        : data{new T[other.size]}, size{other.size} {
      other.was_copied_from_ = true;
      auto o_it = begin(other);
      for (auto it = begin(*this); o_it != end(other); ++it, ++o_it) {
        *it = *o_it;
      }
    }
#endif

    BasicIterable(BasicIterable&& other) : data{other.data}, size{other.size} {
      other.data = nullptr;
      other.was_moved_from_ = true;
    }

    bool was_moved_from() const {
      return this->was_moved_from_;
    }

    bool was_copied_from() const {
      return this->was_copied_from_;
    }

    ~BasicIterable() {
      delete[] this->data;
    }

    template <typename U>
    class Iterator {
     private:
      U* p;

     public:
#ifdef DEFINE_DEFAULT_ITERATOR_CTOR
      Iterator() = default;
#endif
      Iterator(U* b) : p{b} {}
      bool operator!=(const Iterator& other) const {
        return this->p != other.p;
      }

      Iterator& operator++() {
        ++this->p;
        return *this;
      }

      U& operator*() {
        return *this->p;
      }
    };

    friend BasicIterable::Iterator<T> begin(BasicIterable& b) {
      return {b.data};
    }

    friend BasicIterable::Iterator<T> end(BasicIterable& b) {
      return {b.data + b.size};
    }

#ifdef DEFINE_BASIC_ITERABLE_CONST_BEGIN_AND_END
    friend BasicIterable::Iterator<const T> begin(const BasicIterable& b) {
      return {b.data};
    }

    friend BasicIterable::Iterator<const T> end(const BasicIterable& b) {
      return {b.data + b.size};
    }
#endif

#ifdef DECLARE_REVERSE_ITERATOR
    Iterator<T> rbegin();
    Iterator<T> rend();
#endif  // ifdef DECLARE_REVERSE_ITERATOR
  };

  template <typename, typename = void>
  struct IsIterator : std::false_type {};

  template <typename T>
  struct IsIterator<T,
      std::void_t<decltype(T(std::declval<const T&>())),            // copyctor
          decltype(std::declval<T&>() = std::declval<const T&>()),  // copy =
          decltype(*std::declval<T&>()),                            // operator*
          decltype(std::declval<T&>().operator->()),  // operator->
          decltype(++std::declval<T&>()),             // prefix ++
          decltype(std::declval<T&>()++),             // postfix ++
          decltype(
              std::declval<const T&>() != std::declval<const T&>()),  //  !=
          decltype(
              std::declval<const T&>() == std::declval<const T&>())  //  ==,
          >> : std::true_type {};

  template <typename T>
  struct IsForwardIterator
      : std::integral_constant<bool,
            IsIterator<T>::value && std::is_default_constructible<T>::value> {};

  template <typename T>
  struct IsMoveConstructibleOnly
      : std::integral_constant<bool,
            !std::is_copy_constructible<T>::value
                && !std::is_copy_assignable<T>::value
                && !std::is_move_assignable<T>::value
                && std::is_move_constructible<T>::value> {};

  struct Point {
    int x;
    int y;
    int get_x() const {
      return x;
    }
    int get_y() const {
      return y;
    }

    std::string prefix(const std::string& str) const {
      std::ostringstream ss;
      ss << str << "(" << x << ", " << y << ")";
      return ss.str();
    }

    bool operator==(Point other) const {
      return x == other.x && y == other.y;
    }

    bool operator!=(Point other) const {
      return !(*this == other);
    }

    friend std::ostream& operator<<(std::ostream& out, const Point& p) {
      return out << p.prefix("");
    }

    bool left_of(Point other) const {
      return x < other.x;
    }

    Point add(Point other) const {
      return {x + other.x, y + other.y};
    }
  };
}
template <typename T, typename Inc>
class DiffEndRange {
 private:
  T start_;
  T stop_;
  std::vector<T> all_results_;

 public:
  constexpr DiffEndRange(T start, T stop) : start_{start}, stop_{stop} {
    while (start < stop_) {
      all_results_.push_back(start);
      Inc{}(start);
    }
  }

  class Iterator;
  class EndIterator;

  class Iterator {
    using SubIter = typename std::vector<T>::iterator;

   private:
    SubIter it_;
    SubIter end_;

   public:
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;

#ifdef CHAR_RANGE_DEFAULT_CONSTRUCTIBLE
    Iterator() = default;
#endif
    Iterator(SubIter it, SubIter end_it) : it_{it}, end_{end_it} {}

    T& operator*() const {
      return *it_;
    }
    T* operator->() const {
      return &*it_;
    }

    Iterator& operator++() {
      ++it_;
      return *this;
    }

    bool operator!=(const Iterator& other) const {
      return it_ != other.it_;
    }

    bool operator!=(const EndIterator&) const {
      return it_ != end_;
    }

    friend bool operator!=(const EndIterator& lhs, const Iterator& rhs) {
      return rhs != lhs;
    }
  };

  class ReverseIterator {
    using SubIter = typename std::vector<T>::reverse_iterator;

   private:
    SubIter it_;
    SubIter end_;

   public:
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;

#ifdef CHAR_RANGE_DEFAULT_CONSTRUCTIBLE
    ReverseIterator() = default;
#endif
    ReverseIterator(SubIter it, SubIter end_it) : it_{it}, end_{end_it} {}

    T& operator*() const {
      return *it_;
    }
    T* operator->() const {
      return &*it_;
    }

    Iterator& operator++() {
      ++it_;
      return *this;
    }

    bool operator!=(const Iterator& other) const {
      return it_ != other.it_;
    }

    bool operator!=(const EndIterator&) const {
      return it_ != end_;
    }

    friend bool operator!=(const EndIterator& lhs, const Iterator& rhs) {
      return rhs != lhs;
    }
  };

  class EndIterator {};
  class ReverseEndIterator {};

  Iterator begin() {
    return {std::begin(all_results_), std::end(all_results_)};
  }

  EndIterator end() {
    return {};
  }

  ReverseIterator rbegin() {
    return {std::rbegin(all_results_), std::rend(all_results_)};
  }

  ReverseEndIterator rend() {
    return {};
  }
};

struct CharInc {
  void operator()(char& c) {
    ++c;
  }
};

// A range from 'a' to stop, begin() and end() are different
class CharRange : public DiffEndRange<char, CharInc> {
 public:
  constexpr CharRange(char stop) : DiffEndRange<char, CharInc>('a', stop) {}
};

struct IncIntCharPair {
  void operator()(std::pair<int, char>& p) {
    ++p.first;
    ++p.second;
  }
};

class IntCharPairRange
    : public DiffEndRange<std::pair<int, char>, IncIntCharPair> {
 public:
  IntCharPairRange(std::pair<int, char> stop)
      : DiffEndRange<std::pair<int, char>, IncIntCharPair>({0, 'a'}, stop) {}
};

#endif
