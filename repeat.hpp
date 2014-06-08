#ifndef REPEAT_HPP__
#define REPEAT_HPP__

#include <iterator>
#include <utility>

namespace iter {

    // must me negative
    constexpr int INFINITE_REPEAT = -1;

    template <typename T>
    class Repeater;

    template <typename T>
    Repeater<T> repeat(T&&, int count =INFINITE_REPEAT);

    template <typename T>
    class Repeater {
        friend Repeater repeat<T>(T&&, int);
        private:
            T elem;
            int count;

            Repeater(T e, int c)
                : elem(std::forward<T>(e)),
                count{c}
            { }
        public:

            class Iterator {
                private:
                    T& elem;
                    int count;
                public:
                    Iterator(T& e, int c)
                        : elem{e},
                        count{c}
                    { }

                    // count down to 0
                    // INFINITE_REPEAT will be negative, and in that case
                    // the value is never decremented, it will always compare
                    // != to an end iterator
                    Iterator& operator++() {
                        if (this->count > 0) {
                            --this->count;
                        }
                        return *this;
                    }

                    bool operator!=(const Iterator& other) const {
                        return this->count != other.count ||
                            &this->elem != &other.elem;
                    }

                    T& operator*() {
                        return this->elem;
                    }
            };

            Iterator begin() {
                return {this->elem, this->count};
            }

            Iterator end() {
                return {this->elem, 0};
            }

    };

    template <typename T>
    Repeater<T> repeat(T&& e, int count) {
        return {std::forward<T>(e), count};
    }
        
}

#endif //REPEAT_HPP__
