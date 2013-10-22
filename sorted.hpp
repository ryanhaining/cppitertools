#ifndef SORTED__HPP__
#define SORTED__HPP__

#include <algorithm>
#include <vector>

namespace iter {
    template <typename Container, typename CompareFunc>
    class Sorted;

    template <typename Container, typename CompareFunc>
    Sorted<Container, CompareFunc> sorted(Container &, CompareFunc);

    template <typename Container, typename CompareFunc>
    class Sorted {
        private:
            friend Sorted
                sorted<Container, CompareFunc>(Container &, CompareFunc);

            using contained_iter_type =
                decltype(std::declval<Container>().begin());

            std::vector<contained_iter_type> sorted_iters;

            using sorted_iter_type = decltype(sorted_iters.begin());
            using contained_iter_ret = 
                decltype(sorted_iters.begin().operator*().operator*());

            Sorted() = delete;
            Sorted & operator=(const Sorted &) = delete;

            Sorted(Container & container, CompareFunc compare_func) {
                // Fill the sorted_iters vector with an iterator to each
                // element in the container
                for (auto iter = container.begin();
                        iter != container.end();
                        ++iter) {
                    sorted_iters.push_back(iter);
                }

                // sort by comparing the elements that the iterators point to
                std::sort(sorted_iters.begin(), sorted_iters.end(),
                        [&] (const contained_iter_type & it1,
                            const contained_iter_type & it2)
                        { return compare_func(*it1, *it2); });
            }

        public:

            Sorted(const Sorted &) = default;

            // Iterates over a series of Iterators, automatically dereferencing
            // them when accessed with operator *
            class IteratorIterator : public sorted_iter_type {
                public:
                    IteratorIterator(sorted_iter_type iter) :
                        sorted_iter_type(iter)
                    { }
                    IteratorIterator(const IteratorIterator &) = default;

                    // Dereference the current iterator before returning
                    contained_iter_ret operator*() {
                        return *sorted_iter_type::operator*();
                    }
            };

            IteratorIterator begin() {
                IteratorIterator iteriter(sorted_iters.begin());
                return iteriter;
            }

            IteratorIterator end() {
                IteratorIterator iteriter(sorted_iters.end());
                return iteriter;
            }
    };

    template <typename Container, typename CompareFunc>
    Sorted<Container, CompareFunc> sorted(
            Container & container, CompareFunc compare_func) {
        return Sorted<Container, CompareFunc>(container, compare_func);
    }

    template <typename Container>
    auto sorted(Container & container) ->
             decltype(sorted(
                         container,
                         std::less<decltype(
                             std::declval<Container>().begin().operator*())>()
                         ))
     {
         return sorted(
                 container,
                 std::less<decltype(
                     std::declval<Container>().begin().operator*())>()
                 );
    }

}

#endif //#ifndef SORTED__HPP__
