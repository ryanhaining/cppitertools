#include "range.h"

namespace iter {

    // Iterator subclass
    range::Iterator::Iterator (int val, int step, bool invalid_range) :
        value(val),
        step(step),
        invalid_range(invalid_range)
    { }

    int range::Iterator::operator*() const {
        return this->value;
    }

    range::Iterator & range::Iterator::operator++() {
        this->value += this->step;
        return *this;
    }

    bool range::Iterator::operator!=(const range::Iterator & other) const {
        // okay, this is a bit of a hack, but since the for loop will
        // repeatedly check to see if the iterator has reached the end, in
        // order to give a false positive on an invalid range (such as
        // range(-1, 0, -1)) an invalid_range must be checked for first
        return !this->invalid_range && this->value != other.value;
    }


    //Range 
    range::range(int stop) :
        start(0),
        stop(stop),
        step(1)
    {
        this->step_check();
    }

    range::range(int start, int stop) :
        start(start),
        stop(stop),
        step(1)
    {
        this->step_check();
    }

    range::range(int start, int stop, int step) :
        start(start),
        stop(stop),
        step(step)
    {
        this->step_check();
    }

    bool range::invalid_range() const {
       return ((this->start > this->stop && this->step > 0) ||
               (this->start < this->stop && this->step < 0));
    }

    void range::step_check() const throw(RangeException) {
        if (step == 0) {
            throw RangeException();
        }
    }

    range::Iterator range::begin() const {
        return Iterator(start, step, this->invalid_range());
    }

    range::Iterator range::end() const { 
        return Iterator(stop, step, this->invalid_range());
    }

}
