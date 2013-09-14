#ifndef __RANGE__H__
#define __RANGE__H__

// range() for range-based loops with start, stop, and step.
//
// Acceptable forms are:
// for (auto i : range(stop)) { ... } // start = 0, step = 1
// for (auto i : range(start, stop)) { ... } // step = 1
// for (auto i : range(start, stop, step)) { ... }

#include <exception>

namespace iter {

    // Thrown when step 0 occurs
    class RangeError : public std::exception {
        virtual const char *what() const throw() { 
            return "range() step argument must not be zero";
        }
    };

    class range {
        private:
            const int start; 
            const int stop;
            const int step;
            bool invalid_range() const;
            void step_check() const throw(RangeError);
        public:
            class Iterator {
                private:
                    int value;
                    const int step;
                    const bool invalid_range;
                public:
                    Iterator (int val, int step, bool invalid_range);

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
