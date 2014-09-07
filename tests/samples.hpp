#ifndef ITERTOOLS_SAMPLE_CLASSES_HPP
#define ITERTOOLS_SAMPLE_CLASSES_HPP

#include <iostream>

namespace itertest {
    class MoveOnly {
        private:
            int i; // not an aggregate
        public:
            MoveOnly(int v)
                : i{v}
            { }

            MoveOnly(const MoveOnly&) = delete;
            MoveOnly& operator=(const MoveOnly&) = delete;

            MoveOnly(MoveOnly&& other) noexcept
                : i{other.i}
            { }
            MoveOnly& operator=(MoveOnly&& other) noexcept {
                this->i = other.i;
                return *this;
            }
            friend std::ostream& operator<<(
                    std::ostream& out, const MoveOnly& self) {
                return out << self.i;
            }
    };

}
#endif // #ifndef ITERTOOLS_SAMPLE_CLASSES_HPP
