#ifndef TEST_HELPER_H_
#define TEST_HELPER_H_

#include <stdexcept>

namespace itertest {

// move-constructible only int wrapper
class SolidInt {
    private:
        const int i;
        bool moved_from = false;
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

        SolidInt(SolidInt&& other)
            : i{other.i}
        {
            if (other.moved_from) {
                throw std::invalid_argument{
                    "Object was constructed with a move ctor"};
            }
            other.moved_from = true;
        }

};


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

}

#endif
