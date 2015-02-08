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

namespace iter {

    // Thrown when step 0 occurs
    class RangeException : public std::exception {
        const char *what() const noexcept override {
            return "range step must be non-zero";
        }
    };

    template <typename T, bool IsFloat =false>
    class Range;

    template <typename T, bool IsFloat =std::is_floating_point<T>::value>
    Range<T, IsFloat> range(T);
    template <typename T, bool IsFloat =std::is_floating_point<T>::value>
    Range<T, IsFloat> range(T, T);
    template <typename T, bool IsFloat =std::is_floating_point<T>::value>
    Range<T, IsFloat> range(T, T, T);

    // General version for everything not a float
    template <typename T, bool>
    class Range {
        friend Range range<T, false>(T);
        friend Range range<T, false>(T, T);
        friend Range range<T, false>(T, T, T);
        private:
            const T start; 
            const T stop;
            const T step;

            Range(T stop)
                : start{0},
                stop{stop},
                step{1}
            { }

            Range(T start, T stop, T step =1)
                : start{start},
                stop{stop},
                step{step}
            { }

        public:
            class Iterator
                : public std::iterator<std::input_iterator_tag, T>
            {
                private:
                    T value;
                    T step;

                    // compare unsigned values
                    bool not_equal_to(
                            const Iterator& other, std::true_type ) const {
                        return this->value < other.value;
                    }

                    // compare signed values
                    bool not_equal_to(
                            const Iterator& other, std::false_type) const {
                        return !(this->step > 0 && this->value >= other.value) 
                            && !(this->step < 0 && this->value <= other.value);
                    }
                public:
                    Iterator(T val, T step)
                        : value{val},
                        step{step}
                    { }

                    T operator*() const {
                        return this->value;
                    }

                    Iterator& operator++() {
                        this->value += this->step;
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
                    bool operator!=(const Iterator& other) const { 
                        return not_equal_to(
                                other, typename std::is_unsigned<T>::type());
                    }

                    bool operator==(const Iterator& other) const {
                        return !(*this != other);
                    }
            };

            Iterator begin() const {
                return {start, step};
            }

            Iterator end() const { 
                return {stop, step};
            }
    };

    // This specialization is used for floating point types.  Instead of
    // adding one "step" each time ++ is called on the iterator, the value
    // is recalculated as start + (steps_taken + step_size) to avoid
    // accumulating floating point inaccuracies 
    template <typename T>
    class Range<T, true> {
        friend Range range<T, true>(T);
        friend Range range<T, true>(T, T);
        friend Range range<T, true>(T, T, T);
        private:
            const T start; 
            const T stop;
            const T step;

            Range(T stop)
                : start{0},
                stop{stop},
                step{1}
            { }

            Range(T start, T stop, T step =1)
                : start{start},
                stop{stop},
                step{step}
            { }
        public:
            class Iterator
                : public std::iterator<std::input_iterator_tag, T>
            {
                private:
                    T start;
                    T value;
                    T step;
                    unsigned long steps_taken =0;

                public:
                    Iterator(T start, T step)
                        : start{start},
                        value{start},
                        step{step}
                    { }

                    bool operator!=(const Iterator& other) const {
                        return !(this->step > 0 && this->value >= other.value) 
                            && !(this->step < 0 && this->value <= other.value);
                    }

                    bool operator==(const Iterator& other) const {
                        return !(*this != other);
                    }

                    T operator*() const {
                        return this->value;
                    }

                    Iterator& operator++() {
                        ++this->steps_taken;
                        this->value = this->start +
                            (this->step * this->steps_taken);
                        return *this;
                    }

                    Iterator operator++(int) {
                        auto ret = *this;
                        ++*this;
                        return ret;
                    }
            };

            Iterator begin() const {
                return {start, step};
            }

            Iterator end() const { 
                return {stop, step};
            }
    };

    template <typename T, bool IsFloat>
    Range<T, IsFloat> range(T stop) {
        return {stop};
    }

    template <typename T, bool IsFloat>
    Range<T, IsFloat> range(T start, T stop) {
        return {start, stop};
    }

    template <typename T, bool IsFloat>
    Range<T, IsFloat> range(T start, T stop, T step) {
        if (step == 0) {
            throw RangeException{};
        }
        return {start, stop, step};
    }
}

#endif // #ifndef ITER_RANGE_H_
