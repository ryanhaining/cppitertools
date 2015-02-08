#ifndef ITERATOR_ITERATOR_HPP_
#define ITERATOR_ITERATOR_HPP_

#include "iterbase.hpp"
#include "zip.hpp"
#include <iterator>
#include <type_traits>
#include <utility>

// IterIterWrapper and IteratorIterator provide a means to have a container
// of iterators act like a container of the pointed to objects. This is useful
// for combinatorics and similar itertools which need to keep track of
// more than one element at a time.
// an IterIterWrapper<some_collection_type<collection<T>::iterator>>
// behave like some_collection<T> when iterated over or indexed

namespace iter {
    template <typename Iter,
         typename Diff =typename std::iterator_traits<Iter>::difference_type>
    class IteratorIterator : public std::iterator<
        std::random_access_iterator_tag,
        typename std::iterator_traits<Iter>::value_type,
        Diff,
        typename std::iterator_traits<Iter>::pointer,
        typename std::iterator_traits<Iter>::reference
     >
    {
        static_assert(std::is_same<
            typename std::iterator_traits<Iter>::iterator_category,
            std::random_access_iterator_tag>::value,
            "IteratorIterator only works with random access iterators");
        private:
            Iter sub_iter;
        public:
            IteratorIterator() = default;
            IteratorIterator(const Iter& it)
                : sub_iter{it}
            { }

            bool operator==(const IteratorIterator& other) const {
                return !(*this != other);
            }

            bool operator!=(const IteratorIterator& other) const {
                return this->sub_iter != other.sub_iter;
            }

            IteratorIterator& operator++() {
                ++this->sub_iter;
                return *this;
            }

            IteratorIterator operator++(int) {
                auto ret = *this;
                ++*this;
                return ret;
            }

            IteratorIterator& operator--() {
                --this->sub_iter;
                return *this;
            }

            IteratorIterator operator--(int) {
                auto ret = *this;
                --*this;
                return ret;
            }

            auto operator*() -> decltype(**sub_iter) {
                return **this->sub_iter;
            }

            auto operator->() -> decltype(*sub_iter) {
                return *this->sub_iter;
            }


            IteratorIterator& operator+=(Diff n) {
                this->sub_iter += n;
                return *this;
            }

            IteratorIterator operator+(Diff n) const {
                auto it = *this;
                it += n;
                return it;
            }

            friend IteratorIterator operator+(Diff n, IteratorIterator it) {
                it += n;
                return it;
            }

            IteratorIterator& operator-=(Diff n) {
                this->sub_iter -= n;
                return *this;
            }

            IteratorIterator operator-(Diff n) const {
                auto it = *this;
                it -= n;
                return it;
            }

            friend IteratorIterator operator-(Diff n, IteratorIterator it) {
                it -= n;
                return it;
            }

            Diff operator-(const IteratorIterator& rhs) const {
                return this->sub_iter - rhs.sub_iter;
            }

            auto operator[](Diff idx) -> decltype(*sub_iter[idx]) {
                return *sub_iter[idx];
            }

            bool operator<(const IteratorIterator& rhs) const {
                return this->sub_iter < rhs.sub_iter;
            }

            bool operator>(const IteratorIterator& rhs) const {
                return this->sub_iter > rhs.sub_iter;
            }

            bool operator<=(const IteratorIterator& rhs) const {
                return this->sub_iter <= rhs.sub_iter;
            }

            bool operator>=(const IteratorIterator& rhs) const {
                return this->sub_iter >= rhs.sub_iter;
            }
    };

    template <typename Container>
    class IterIterWrapper {
        private:
            Container container;

            using contained_iter = typename Container::value_type;
            using size_type = typename Container::size_type;
            using iterator =
                IteratorIterator<typename Container::iterator>;
            using reverse_iterator =
                IteratorIterator<typename Container::reverse_iterator>;

        public:
            IterIterWrapper() = default;

            explicit IterIterWrapper(size_type sz)
                : container(sz)
            { }

            IterIterWrapper(size_type sz, const contained_iter& val)
                : container(sz, val)
            { }

            auto at(size_type pos) -> decltype(*container.at(pos)) {
                return *container.at(pos);
            }

            auto at(size_type pos) const -> decltype(*container.at(pos)) {
                return *container.at(pos);
            }

            auto operator[](size_type pos) 
                noexcept(noexcept(*container[pos]))
                -> decltype(*container[pos])
            {
                return *container[pos];
            }

            auto operator[](size_type pos) const
                noexcept(noexcept(*container[pos]))
                -> decltype(*container[pos])
            {
                return *container[pos];
            }

            bool empty() const noexcept {
                return container.empty();
            }

            size_type size() const noexcept {
                return container.size();
            }

            iterator begin() noexcept {
                return {container.begin()};
            }

            iterator end() noexcept {
                return {container.end()};
            }

            reverse_iterator rbegin() noexcept {
                return {container.rbegin()};
            }

            reverse_iterator rend() noexcept {
                return {container.rend()};
            }

            // get() exposes the underlying container.  this allows the
            // itertools to manipulate the iterators in the container
            // and should not be depended on anywhere else.
            Container& get() noexcept {
                return container;
            }

            const Container& get() const noexcept {
                return container;
            }

    };
}

#endif
