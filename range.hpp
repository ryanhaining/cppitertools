#ifndef ITER_RANGE_H_
#define ITER_RANGE_H_

// range() for range-based loops with start, stop, and step.
//
// Acceptable forms are:
// for (auto i : range(stop)) { ... } // start = 0, step = 1
// for (auto i : range(start, stop)) { ... } // step = 1
// for (auto i : range(start, stop, step)) { ... }
//
// The start may be greater than the stop if the range is negative
// The range will effectively be empty if:
//   1) step is positive and start > stop
//   2) step is negative and start < stop
//
// If a step of 0 is provided, a RangeException will be thrown

#include <exception>
#include <type_traits>
#include <iterator>
#include <cassert>

namespace iter {

    template <typename T, bool IsFloat=std::is_floating_point<T>::value>
    class RangeIterData;

    // everything except floats
    template <typename T>
    class RangeIterData<T, false> {
        private:
            T value_{};
            T step_{};
        public:
            RangeIterData() =default;
            RangeIterData(T in_value, T in_step)
                : value_{in_value},
                step_{in_step}
            { }

            T value() const {
                return this->value_;
            }

            T step() const {
                return this->step_;
            }

            void inc() {
                this->value_ += step_;
            }

            bool operator==(const RangeIterData& other) const {
              return this->value_ == other.value_;
            }

            bool operator!=(const RangeIterData& other) const {
              return !(*this == other);
            }
    };

    // float data
    template <typename T>
    class RangeIterData<T, true> {
        private:
            T start_{};
            T value_{};
            T step_{};
            unsigned long steps_taken{};
        public:
            RangeIterData() =default;
            RangeIterData(T in_start, T in_step)
                : start_{in_start},
                value_{in_start},
                step_{in_step}
            { }

            T value() const {
                return this->value_;
            }

            T step() const {
                return this->step_;
            }

            void inc() {
                ++this->steps_taken;
                value_ = this->start_ +
                        (this->step_ * this->steps_taken);
            }

            bool operator==(const RangeIterData& other) const {
                // if the difference between the two values is less than the
                // step size, they are considered equal
                T diff = this->value_ < other.value_ ?
                    other.value_ - this->value_ : this->value_ - other.value_;
                return diff < step_;
            }

            bool operator!=(const RangeIterData& other) const {
              return !(*this == other);
            }
    };


    // Thrown when step 0 occurs
    class RangeException : public std::exception {
        const char *what() const noexcept override {
            return "range step must be non-zero";
        }
    };

    template <typename T>
    class Range;

    template <typename T>
    Range<T> range(T);
    template <typename T>
    Range<T> range(T, T);
    template <typename T>
    Range<T> range(T, T, T);

    // General version for everything not a float
    template <typename T>
    class Range {
        friend Range range<T>(T);
        friend Range range<T>(T, T);
        friend Range range<T>(T, T, T);
        private:
            const T start;
            const T stop;
            const T step;

            Range(T in_stop)
                : start{0},
                stop{in_stop},
                step{1}
            { }

            Range(T in_start, T in_stop, T in_step =1)
                : start{in_start},
                stop{in_stop},
                step{in_step}
            { }

        public:
            // the reference type here is T, which doesn't strictly follow all
            // of the rules, but std::vector<bool>::iterator::reference isn't
            // a reference type either, this isn't any worse

            class Iterator
                : public std::iterator<
                  std::forward_iterator_tag,
                  T,
                  std::ptrdiff_t,
                  T*,
                  T>
            {
                private:
                    RangeIterData<T> data;
                    bool is_end;

                    // compare unsigned values
                    static bool not_equal_to_impl(
                            const Iterator& iter, const Iterator& end_iter,
                            std::true_type ) {
                        assert(!iter.is_end);
                        assert(end_iter.is_end);
                        return iter.data.value() < end_iter.data.value();
                    }

                    // compare signed values
                    static bool not_equal_to_impl(
                            const Iterator& iter, const Iterator& end_iter,
                            std::false_type) {
                        assert(!iter.is_end);
                        assert(end_iter.is_end);
                        return !(iter.data.step() > 0 && iter.data.value()
                                 >= end_iter.data.value())
                            && !(iter.data.step() < 0 && iter.data.value()
                                 <= end_iter.data.value());
                    }

                    static bool not_equal_to_end(
                            const Iterator& lhs, const Iterator& rhs) {
                        if (rhs.is_end) {
                            return not_equal_to_impl(
                                    lhs, rhs, std::is_unsigned<T>{});
                        } else {
                            return not_equal_to_impl(
                                    rhs, lhs, std::is_unsigned<T>{});
                        }
                    }

                public:
                    Iterator() =default;

                    Iterator(T in_value, T in_step, bool in_is_end)
                        : data(in_value, in_step),
                        is_end{in_is_end}
                    { }

                    T operator*() const {
                        return this->data.value();
                    }

                    Iterator& operator++() {
                        this->data.inc();
                        return *this;
                    }

                    Iterator operator++(int) {
                        auto ret = *this;
                        ++*this;
                        return ret;
                    }

                    // This operator would more accurately read as "in bounds"
                    // or "incomplete" because exact comparison with the end
                    // isn't good enough for the purposes of this Iterator.
                    // There are two odd cases that need to be handled
                    //
                    // 1) The Range is infinite, such as
                    // Range (-1, 0, -1) which would go forever down toward
                    // infinitely (theoretically).  If this occurs, the Range
                    // will instead effectively be empty
                    //
                    // 2) (stop - start) % step != 0.  For
                    // example Range(1, 10, 2).  The iterator will never be
                    // exactly equal to the stop value.
                    //
                    // Another way to think about it is that the "end"
                    // iterator represents the range of values that are invalid
                    // So, if an iterator is not equal to that, it is valid
                    //
                    // Two end iterators will compare equal
                    //
                    // Two non-end iterators will compare by their stored values
                    bool operator!=(const Iterator& other) const {
                        if (this->is_end && other.is_end) {
                            return false;
                        }

                        if (!this->is_end && !other.is_end) {
                            return this->data != other.data;
                        }
                        return not_equal_to_end(*this, other);
                    }

                    bool operator==(const Iterator& other) const {
                        return !(*this != other);
                    }
            };

            Iterator begin() const {
                return {start, step, false};
            }

            Iterator end() const {
                return {stop, step, true};
            }
    };

    template <typename T>
    Range<T> range(T stop) {
        return {stop};
    }

    template <typename T>
    Range<T> range(T start, T stop) {
        return {start, stop};
    }

    template <typename T>
    Range<T> range(T start, T stop, T step) {
        if (step == 0) {
            throw RangeException{};
        }
        return {start, stop, step};
    }
}

#endif // #ifndef ITER_RANGE_H_
