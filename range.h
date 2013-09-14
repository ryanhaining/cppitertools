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
            return "range() step argument must not be zero";
        }
    };

    class range {
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

            range(int stop);
            range(int start, int stop);
            range(int start, int stop, int step);

            Iterator begin() const;
            Iterator end() const;
    };
}

#endif //ifndef __RANGE__H__
