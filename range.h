#ifndef __RANGE__H__
#define __RANGE__H__

// range() for range-based loops with start, stop, and step.
//
// Acceptable forms are:
// for (auto i : range(stop)) { ... } // start = 0, step = 1
// for (auto i : range(start, stop)) { ... } // step = 1
// for (auto i : range(start, stop, step)) { ... }

namespace iter {
    class range {
        private:
            unsigned int start; 
            unsigned int stop;
            unsigned int step;
        public:
            class Iterator {
                private:
                    unsigned int value;
                    unsigned int step;
                public:
                    Iterator (unsigned int val, unsigned int step);

                    unsigned int operator*() const;
                    Iterator & operator++();
                    bool operator!=(const Iterator & other) const;
            };

            range(unsigned int stop);
            range(unsigned int start, unsigned int stop);
            range(unsigned int start, unsigned int stop, unsigned int step);

            Iterator begin() const;
            Iterator end() const;
    };
}

#endif //ifndef __RANGE__H__
