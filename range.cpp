#include "range.h"

namespace iter {

    // Iterator subclass
    range::Iterator::Iterator (unsigned int val, unsigned int step) :
        value(val),
        step(step) { }

    unsigned int range::Iterator::operator*() const {
        return this->value;
    }

    range::Iterator & range::Iterator::operator++() {
        this->value += this->step;
        return *this;
    }

    bool range::Iterator::operator!=(const range::Iterator & other) const {
        return this->value != other.value;
    }


    //Range 
    range::range(unsigned int stop) :
        start(0),
        stop(stop),
        step(1) { }

    range::range(unsigned int start, unsigned int stop) :
        start(start),
        stop(stop),
        step(1) { }

    range::range(unsigned int start, unsigned int stop, unsigned int step) :
        start(start),
        stop(stop),
        step(step) { }

    range::Iterator range::begin() const {
        return Iterator(start, step);
    }

    range::Iterator range::end() const { 
        return Iterator(stop, step);
    }

}
