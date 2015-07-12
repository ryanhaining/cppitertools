#ifndef ITER_REPEAT_HPP_
#define ITER_REPEAT_HPP_

#include <iterator>
#include <type_traits>
#include <utility>

namespace iter {
    template <typename T>
    class RepeaterWithCount ;

    template <typename T>
    constexpr RepeaterWithCount<T> repeat(T&&, int);

    template <typename T>
    class RepeaterWithCount {
        friend RepeaterWithCount repeat<T>(T&&, int);
        private:
            T elem;
            int count;

            constexpr RepeaterWithCount(T e, int c)
                : elem(std::forward<T>(e)),
                count{c}
            { }

            using TPlain = typename std::remove_reference<T>::type;
        public:
            class Iterator
                : public std::iterator<std::input_iterator_tag, const TPlain>
            {
                private:
                    const TPlain *elem;
                    int count;
                public:
                    constexpr Iterator(const TPlain* e, int c)
                        : elem{e},
                        count{c}
                    { }

                    Iterator& operator++() {
                        --this->count;
                        return *this;
                    }

                    Iterator operator++(int) {
                        auto ret = *this;
                        ++*this;
                        return ret;
                    }

                    constexpr bool operator!=(const Iterator& other) const {
                        return !(*this == other);
                    }

                    constexpr bool operator==(const Iterator& other) const {
                        return this->count == other.count;
                    }

                    constexpr const TPlain& operator*() const {
                        return *this->elem;
                    }

                    constexpr const TPlain* operator->() const {
                        return this->elem;
                    }
            };

            constexpr Iterator begin() {
                return {&this->elem, this->count};
            }

            constexpr Iterator end() {
                return {&this->elem, 0};
            }

    };
        
    template <typename T>
    constexpr RepeaterWithCount<T> repeat(T&& e, int count) {
        return {std::forward<T>(e), count < 0 ? 0 : count};
    }

    template <typename T>
    class Repeater;

    template <typename T>
    constexpr Repeater<T> repeat(T&&);

    template <typename T>
    class Repeater{
        friend Repeater repeat<T>(T&&);
        private:
            using TPlain = typename std::remove_reference<T>::type;
            T elem;

            constexpr Repeater(T e)
                : elem(std::forward<T>(e))
            { }
        public:
            class Iterator
                : public std::iterator<std::input_iterator_tag, const TPlain>
            {
                private:
                    const TPlain *elem;

                public:
                    constexpr Iterator(const TPlain* e)
                        : elem{e}
                    { }

                    constexpr const Iterator& operator++() const {
                        return *this;
                    }

                    constexpr Iterator operator++(int) const {
                        return *this;
                    }

                    constexpr bool operator!=(const Iterator&) const {
                        return true;
                    }

                    constexpr bool operator==(const Iterator&) const {
                        return false;
                    }

                    constexpr const TPlain& operator*() const {
                        return *this->elem;
                    }

                    constexpr const TPlain* operator->() const {
                        return this->elem;
                    }
            };


            constexpr Iterator begin() {
                return {&this->elem};
            }

            constexpr Iterator end() {
                return {nullptr};
            }
    };

    template <typename T>
    constexpr Repeater<T> repeat(T&& e) {
        return {std::forward<T>(e)};
    }
}

#endif
