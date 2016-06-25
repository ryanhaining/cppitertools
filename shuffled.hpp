#ifndef ITER_SHUFFLED_HPP_
#define ITER_SHUFFLED_HPP_

#include "internal/iterbase.hpp"
#include <iterator>
#include <array>
#include <limits>

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
    template <typename Container, typename Distance = std::size_t>
    class ShuffledView;
    // linear feedback shift register
    namespace lfsr {
      constexpr std::array<std::uint16_t, 64> PRIME_POLY = {
          {1, 3, 3, 3, 5, 3, 3, 29, 17, 9, 5, 83, 27, 43, 3, 45, 9, 129, 39, 9,
              5, 3, 33, 27, 9, 71, 39, 9, 5, 83, 9, 197, 8193, 231, 5, 119, 83,
              99, 17, 57, 9, 153, 89, 101, 27, 449, 33, 183, 113, 29, 75, 9, 71,
              125, 71, 149, 129, 99, 123, 3, 39, 105, 3,
              27}};  // prime poly for full-cycle shift registers
      constexpr std::uint16_t get_approx(std::uint64_t val);
      constexpr std::uint64_t shift(std::uint64_t reg, std::uint8_t reg_size);
    }
  }

  template <typename Container, typename Distance = std::size_t>
  impl::ShuffledView<Container, Distance> shuffled(Container&&, int seed = 1);
}

// power of 2 approximation (val < pow(2, get_approx(val)+1))
constexpr inline std::uint16_t iter::impl::lfsr::get_approx(std::uint64_t val) {
  if (val == 0) {
    return 0;
  }
  constexpr auto LIMIT = std::uint64_t(1UL) << 63;
  if (val > LIMIT) {
    return 63;
  }
  std::uint16_t pow2_approx = 0;
  while (pow2_approx < 64 && (val >= (1UL << (pow2_approx + 1)))) {
    ++pow2_approx;
  }
  return pow2_approx;
}

constexpr inline std::uint64_t iter::impl::lfsr::shift(
    std::uint64_t reg, std::uint8_t reg_size) {
  if (reg & 1) {
    reg = ((reg ^ PRIME_POLY[reg_size]) >> 1) | (1 << reg_size);
  } else {
    reg >>= 1;
  }
  return reg;
}

template <typename Container, typename Distance>
class iter::impl::ShuffledView {
 private:
  Container container;
  Distance size;
  std::uint8_t size_approx;
  std::uint64_t seed;

  friend ShuffledView iter::shuffled<Container, Distance>(Container&&, int);

  using IterDeref = iterator_traits_deref<Container>;
  ShuffledView(Container&& in_container, int in_seed)
      : container(std::forward<Container>(in_container)),
        size(std::distance(std::begin(container), std::end(container))),
        size_approx(lfsr::get_approx(size)),
        seed(in_seed) {
    if (size > 0) {
      std::uint64_t mask = -1;
      mask = (mask >> (64 - size_approx));
      this->seed = seed & mask;
      this->seed = lfsr::shift(this->seed, size_approx);
      while (this->seed >= size) {
        this->seed = lfsr::shift(this->seed, size_approx);
      }
    }
  }

 public:
  ShuffledView(ShuffledView&&) = default;

  class Iterator : public std::iterator<std::input_iterator_tag, IterDeref> {
   private:
    friend class ShuffledView;
    ShuffledView* owner{};
    std::uint64_t state{};

   public:
    Iterator() = default;
    Iterator(ShuffledView* in_owner, std::uint64_t in_state)
        : owner{in_owner}, state{in_state} {}

    Iterator& operator++() {
      if (*this == owner->end()) {
        return *this;
      }
      do {
        state = lfsr::shift(state, owner->size_approx);
      } while (state > owner->size);
      if (state == owner->seed) {
        *this = owner->end();
      }
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    bool operator==(const Iterator& other) const {
      return owner == other.owner && state == other.state;
    }

    bool operator!=(const Iterator& other) const {
      return !(*this == other);
    }

    iterator_deref<Container> operator*() {
      return *std::next(std::begin(owner->container), state - 1);
    }

    ArrowProxy<IterDeref> operator->() {
      return {**this};
    }
  };

  Iterator begin() {
    return {this, (size == 0 ? 0 : seed)};
  }

  Iterator end() {
    return {this, 0};
  }

  /* @brief restore iteration through a vector in shuffled order from the
   * index of the last seen element in non shuffled container
   *
   * @example:
   *   v = {"apple", "banana", "orange", "peach"}; // original vector
   *   s = shuffled(v); // {"orange", "peach", "banana", "apple"} - shuffled
   * vector
   *
   *   We iterated through s. Last seen element was "banana". In the
   *   original vector "banana" had index 1. So to restore shuffling wee
   *   need to do s.restore(1); Operation is zero cost - no iterators
   *   advance is needed.*/
  Iterator restore(std::uint64_t state) {
    return {this, (state >= size ? seed : state + 1)};
  }
};


template <typename Container, typename Distance = std::size_t>
iter::impl::ShuffledView<Container, Distance> iter::shuffled(
    Container&& container, int seed) {
  return {std::forward<Container>(container), seed};
}

#endif
