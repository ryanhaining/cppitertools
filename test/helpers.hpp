#ifndef TEST_HELPER_H_
#define TEST_HELPER_H_

#include <iterbase.hpp>

#include <stdexcept>
#include <type_traits>
#include <utility>

namespace itertest {

// non-copyable. non-movable. non-default-constructible
class SolidInt {
    private:
        const int i;
    public:
        constexpr SolidInt(int n)
            : i{n}
        { }

        constexpr int getint() const {
            return this->i;
        }

        SolidInt() = delete;
        SolidInt(const SolidInt&) = delete;
        SolidInt& operator=(const SolidInt&) = delete;
        SolidInt& operator=(SolidInt&&) = delete;
        SolidInt(SolidInt&&) = delete;
};

namespace {
    struct DoubleDereferenceError : std::exception {
        const char *what() const noexcept override {
            return "Iterator dereferenced twice without increment";
        }
    };

    // this class's iterator will throw if it's dereference twice without
    // an increment in between
    class InputIterable {
        public:
            class Iterator {
                private:
                    int i;
                    bool was_incremented = true;

                public:
                    Iterator(int n)
                        : i{n}
                    { }

                    Iterator& operator++() {
                        ++this->i;
                        this->was_incremented = true;
                        return *this;
                    }

                    int operator*() {
                        if (!this->was_incremented) {
                            throw DoubleDereferenceError{};
                        }
                        this->was_incremented = false;
                        return this->i;
                    }

                    bool operator!=(const Iterator& other) const {
                        return this->i != other.i;
                    }
            };

            Iterator begin() {
                return {0};
            }

            Iterator end() {
                return {5};
            }
    };
}



// BasicIterable provides a minimal forward iterator
// operator++(), operator!=(const BasicIterable&), operator*()
// move constructible only
// not copy constructible, move assignable, or copy assignable
template <typename T>
class BasicIterable {
    private:
        T *data;
        std::size_t size;
        bool was_moved_from_ = false;
        bool was_copied_from_ = false;
    public:
        BasicIterable(std::initializer_list<T> il)
            : data{new T[il.size()]},
            size{il.size()}
        {
            // would like to use enumerate, can't because it's for unit
            // testing enumerate
            std::size_t i = 0;
            for (auto&& e : il) {
                data[i] = e;
                ++i;
            }
        }

        BasicIterable& operator=(BasicIterable&&) = delete;
        BasicIterable& operator=(const BasicIterable&) = delete;

        BasicIterable(const BasicIterable&) = delete;
#if 0
        BasicIterable(const BasicIterable& other)
            : data{new T[other.size()]},
            size{other.size}
        { 
            for (auto it = this->begin(), o_it = other.begin();
                    o_it != other.end();
                    ++it, ++o_it) {
                *it = *o_it;
            }
        }
#endif


        BasicIterable(BasicIterable&& other)
            : data{other.data},
            size{other.size}
        {
            other.data = nullptr;
            other.was_moved_from_ = true;
        }

        bool was_moved_from() const {
            return this->was_moved_from_;
        }

        bool was_copied_from() const {
            return this->was_copied_from_;
        }

        ~BasicIterable() {
            delete [] this->data;
        }

        class Iterator {
            private:
                T *p;
            public:
                Iterator(T *b) : p{b} { }
                bool operator!=(const Iterator& other) const {
                    return this->p != other.p;
                }

                Iterator& operator++() {
                    ++this->p;
                    return *this;
                }

                T& operator*() {
                    return *this->p;
                }
        };

        Iterator begin() {
            return {this->data};
        }

        Iterator end() {
            return {this->data + this->size};
        }
};

using iter::void_t;

template <typename, typename =void>
struct IsIterator : std::false_type { };

template <typename T>
struct IsIterator <T, void_t<
        decltype(T(std::declval<const T&>())), // copyctor
        decltype(std::declval<T&>() = std::declval<const T&>()), // copy =
        decltype(*std::declval<T&>()), // operator*
        decltype(++std::declval<T&>()), // prefix ++
        decltype(std::declval<T&>()++), // postfix ++
        decltype(std::declval<const T&>() != std::declval<const T&>()), //  !=
        decltype(std::declval<const T&>() == std::declval<const T&>()) //  ==
    >> : std::true_type { };

template <typename T>
struct IsForwardIterator : std::integral_constant<bool,
  IsIterator<T>::value && std::is_default_constructible<T>::value> { };

}

#endif
