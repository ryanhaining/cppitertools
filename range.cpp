#include "range.h"

namespace iter {

    // Iterator subclass
    range::Iterator::Iterator (int val, int step) :
        value(val),
        step(step)
    { }

    int range::Iterator::operator*() const {
        return this->value;
    }

    range::Iterator & range::Iterator::operator++() {
        this->value += this->step;
        return *this;
    }

    // This operator would more accurately read as "in bounds" or "incomplete"
    // because exact comparison with the end isn't good enough for the purposes
    // of this Iterator.  
    // There are two odd cases that need to be handled
    // 1) The range is infinite, such as range (-1, 0, -1) which would go
    //    forever down toward infinitely (theoretically).  If this occurs,
    //    the range will instead effectively be empty
    // 2) (stop - start) % step != 0.  For example range(1, 10, 2).  The
    //    iterator will never be exactly equal to the stop value.
    bool range::Iterator::operator!=(const range::Iterator & other) const {
        return !(this->step > 0 && this->value >= other.value) 
            && !(this->step < 0 && this->value <= other.value);
    }


    //Range 
    range::range(int stop) :
        start(0),
        stop(stop),
        step(1)
    { }

    range::range(int start, int stop) :
        start(start),
        stop(stop),
        step(1)
    { }

    range::range(int start, int stop, int step) :
        start(start),
        stop(stop),
        step(step)
    {
        this->step_check();
    }

    void range::step_check() const throw(RangeException) {
        if (step == 0) {
            throw RangeException();
        }
    }

    range::Iterator range::begin() const {
        return Iterator(start, step);
    }

    range::Iterator range::end() const { 
        return Iterator(stop, step);
    }

}
