#ifndef __RANGE__H__
#define __RANGE__H__

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

namespace iter {

    // Thrown when step 0 occurs
    class RangeException : public std::exception {
        virtual const char *what() const throw() { 
            return "Range() step argument must not be zero";
        }
    };

    template <typename T>
    class Range {
        private:
            const int start; 
            const int stop;
            const int step;
            void step_check() const throw(RangeException);


        public:
            class Iterator {
                private:
                    int value;
                    const int step;
                public:
                    Iterator (int val, int step);

                    int operator*() const;
                    Iterator & operator++();
                    bool operator!=(const Iterator & other) const;
            };

            //Range 
            Range::Range(int stop) :
                start(0),
                stop(stop),
                step(1)
            { }
            Range(int start, int stop);
            Range(int start, int stop, int step);

            Range() = delete;
            Range(const Range &) = default;

            Iterator begin() const;
            Iterator end() const;
    };

    // definitions

    // Iterator subclass
    Range::Iterator::Iterator (int val, int step) :
        value(val),
        step(step)
    { }

    int Range::Iterator::operator*() const {
        return this->value;
    }

    Range::Iterator & Range::Iterator::operator++() {
        this->value += this->step;
        return *this;
    }

    // This operator would more accurately read as "in bounds" or "incomplete"
    // because exact comparison with the end isn't good enough for the purposes
    // of this Iterator.  
    // There are two odd cases that need to be handled
    // 1) The Range is infinite, such as Range (-1, 0, -1) which would go
    //    forever down toward infinitely (theoretically).  If this occurs,
    //    the Range will instead effectively be empty
    // 2) (stop - start) % step != 0.  For example Range(1, 10, 2).  The
    //    iterator will never be exactly equal to the stop value.
    bool Range::Iterator::operator!=(const Range::Iterator & other) const {
        return !(this->step > 0 && this->value >= other.value) 
            && !(this->step < 0 && this->value <= other.value);
    }



    Range::Range(int start, int stop) :
        start(start),
        stop(stop),
        step(1)
    { }

    Range::Range(int start, int stop, int step) :
        start(start),
        stop(stop),
        step(step)
    {
        this->step_check();
    }

    void Range::step_check() const throw(RangeException) {
        if (step == 0) {
            throw RangeException();
        }
    }

    Range::Iterator Range::begin() const {
        return Iterator(start, step);
    }

    Range::Iterator Range::end() const { 
        return Iterator(stop, step);
    }

}

template <typename T>
Range<t> range

#endif //ifndef __RANGE__H__
