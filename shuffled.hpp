#ifndef ITER_SHUFFLED_HPP_
#define ITER_SHUFFLED_HPP_

#include "internal/iterbase.hpp"

/* Suppose the size of the container is N. We can find the power of two,
 * that is greater or equal to N - K. Numbers from 1 to K can be easy
 * shuffled with help of Linear Feedback Shift Register (LFSR) using
 * special prime poly. Access to every element of the shuffled is
 * implemented through advance of the first (begin) iterator of the
 * container, so random access iterator is desirable. In the constructor
 * we have to calculate the total size of the container, so
 * std::distance will be used. User can define more effective
 * std::distance for the container.*/

namespace iter {
  namespace impl {
    template <typename Container>
    class ShuffledView;
    // linear feedback shift register
    namespace lfsr {
      const uint16_t PRIME_POLY[64] = {
        1, 3, 3, 3, 5, 3, 3, 29, 17, 9, 5, 83, 27, 43, 3, 45, 9, 129, 39,
        9, 5, 3, 33, 27, 9, 71, 39, 9, 5, 83, 9, 197, 8193, 231, 5, 119,
        83, 99, 17, 57, 9, 153, 89, 101, 27, 449, 33, 183, 113, 29, 75,
        9, 71, 125, 71, 149, 129, 99, 123, 3, 39, 105, 3, 27
      }; // prime poly for full-cycle shift registers
      uint16_t get_approx(uint64_t val);
      uint64_t shift(uint64_t reg, uint8_t reg_size);
    }
  }

  template <typename Container>
  impl::ShuffledView<Container> shuffled(Container&& container, int seed = 1)  {
    return {std::forward<Container>(container), seed};
  }
}

// power of 2 approximation (val < pow(2, get_approx(val)+1))
inline uint16_t
iter::impl::lfsr::get_approx(uint64_t val) {
  if (val == 0)
    return 0;
  uint16_t pow2_approx = 0;
  if (val > 9223372036854775808ULL) {
    pow2_approx = 63;
  }
  else {
    while (pow2_approx < 64) {
      if (val >= (static_cast<uint64_t>(1) << (pow2_approx + 1)))
        ++pow2_approx;
      else
        break;
    }
  }
  return pow2_approx;
}

inline uint64_t
iter::impl::lfsr::shift(uint64_t reg, uint8_t reg_size) {
  if (reg & 1) {
    reg = ((reg ^ PRIME_POLY[reg_size]) >> 1) |
      (static_cast<uint64_t>(1) << reg_size);
  }
  else {
    reg >>= 1;
  }
  return reg;
}

template <typename Container>
class iter::impl::ShuffledView {
 private:

  uint64_t                 size;
  uint8_t                  size_approx;
  iterator_type<Container> in_begin;
  uint64_t                 seed;
  Container                container;

  template <typename C>
  friend ShuffledView<C> iter::shuffled(C&&, int);


 public:
  using IterDeref = typename std::remove_reference<iterator_deref<Container>>;
  ShuffledView(ShuffledView&& copy)
      : size(0), container(std::forward<Container>(copy.container)) {};
  ShuffledView(Container&& container, int seed)
      : size(std::distance(std::begin(container), std::end(container))),
        size_approx(lfsr::get_approx(size)),
        in_begin(std::begin(container)), seed(seed),
        container(std::forward<Container>(container)) {
    if (size == 1) {
      this->seed = 1;
    }
    else if (size > 1) {
      uint64_t mask = 0xFFFFFFFFFFFFFFFFULL;
      mask = (mask >> (64-(size_approx+1)));
      this->seed = seed & mask;
      this->seed = lfsr::shift(this->seed, size_approx);
      while(this->seed > size)
        this->seed = lfsr::shift(this->seed, size_approx);
    }
  }

  class Iterator {
  private:
    friend class ShuffledView<Container>;
    ShuffledView<Container>* owner;
    uint64_t state;
    iterator_type<Container> copy; // referenced by operator* value

  public:
    using iterator_category = std::input_iterator_tag;
    using value_type = iterator_traits_deref<Container>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    Iterator() : owner(nullptr), state(0) {}
    Iterator(const Iterator& other) { operator=(other); }
    Iterator& operator=(const Iterator& other) {
      owner = other.owner;
      state = other.state;
      return *this;
    };

    Iterator& operator++() {
      state = lfsr::shift(state, owner->size_approx);
      while(state > owner->size)
        state = lfsr::shift(state, owner->size_approx);
      if (state == owner->seed)
        operator=(owner->end());
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    bool operator==(const Iterator& other) const {
      return  owner == other.owner && state == other.state;
    }

    bool operator!=(const Iterator& other) const {
      return !operator==(other);
    }

    auto& operator*() {
      copy = owner->in_begin;
      copy = std::next(copy, static_cast<uint64_t>(state-1));
      return *copy;
    }

    auto operator->() -> ArrowProxy<decltype(**this)>{
      return {**this};
    }
  };

  Iterator begin() {
    Iterator begin;
    begin.owner = this;
    begin.state = (size == 0 ? 0 : seed);
    return begin;
  }

  Iterator end() {
    Iterator end;
    end.owner = this;
    end.state = 0;
    return end;
  }

  /* @brief restore iteration through a vector in shuffled order from the
   * index of the last seen element in non shuffled container
   *
   * @example:
   *   v = {"apple", "banana", "orange", "peach"}; // original vector
   *   s = shuffled(v); // {"orange", "peach", "banana", "apple"} - shuffled vector
   *
   *   We iterated through s. Last seen element was "banana". In the
   *   original vector "banana" had index 1. So to restore shuffling wee
   *   need to do s.restore(1); Operation is zero cost - no iterators
   *   advance is needed.*/
  Iterator restore(uint64_t state) {
    Iterator rs;
    rs.owner = this;
    rs.state = (state >= size ? seed : state + 1);
    return rs;
  }
};

#endif

