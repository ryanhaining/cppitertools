#ifndef ITERTOOLS_SAMPLE_CLASSES_HPP
#define ITERTOOLS_SAMPLE_CLASSES_HPP

#include <iostream>
#include <utility>
#include <cstddef>

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

            // for std::next_permutation compatibility
            friend bool operator<(const MoveOnly& lhs, const MoveOnly& rhs) {
                return lhs.i < rhs.i;
            }

            friend std::ostream& operator<<(
                    std::ostream& out, const MoveOnly& self) {
                return out << self.i;
            }

    };

    class DerefByValue {
        private:
            static constexpr std::size_t N = 3;
            int array[N] = {0, 1, 2};
        public:
            DerefByValue() = default;

            class Iterator {
                private:
                    int *current;
                public:
                    Iterator() = default;
                    Iterator(int *p)
                        : current{p}
                    { }

                    bool operator!=(const Iterator& other) const {
                        return this->current != other.current;
                    }

                    // for testing, iterator derefences to an int instead of
                    // an int&
                    int operator*() const {
                        return *this->current;
                    }

                    Iterator& operator++() {
                        ++this->current;
                        return *this;
                    }
            };

            Iterator begin() {
                return {this->array};
            }

            Iterator end() {
                return {this->array + N};
            }
    };

    class DerefByValueFancy {
        private:
            static constexpr std::size_t N = 3;
            int array[N] = {0, 1, 2};
        public:
            DerefByValueFancy() = default;

            int *begin() {
                return this->array;
            }

            int *end() {
                return this->array + N;
            }
    };
}
#endif // #ifndef ITERTOOLS_SAMPLE_CLASSES_HPP
