#ifndef SORTED__HPP__
#define SORTED__HPP__

#include <algorithm>
#include <vector>

namespace iter {

    template <typename Container>
    class Sorted;

    template <typename Container>
    Sorted<Container> sorted(Container &);

    template <typename Container>
    class Sorted {
        private:
            friend Sorted sorted<Container>(Container &);

            using contained_iter_type =
                decltype(std::declval<Container>().begin());

            std::vector<contained_iter_type> sorted_iters;

            Sorted() = delete;
            Sorted & operator=(const Sorted &) = delete;

            Sorted(Container & container) {
                for (auto iter = container.begin();
                        iter != container.end();
                        ++iter) {
                    sorted_iters.push_back(iter);
                }
                std::sort(sorted_iters.begin(), sorted_iters.end(),
                        [] (const contained_iter_type & it1,
                            const contained_iter_type & it2)
                        { return *it1 < *it2; });
            }


        public:
            Sorted(const Sorted &) = default;
            auto begin() const -> decltype(sorted_iters.begin()) {
                return sorted_iters.begin();
            }
            auto end() const -> decltype(sorted_iters.end()) {
                return sorted_iters.end();
            }

    };

    template <typename Container>
    Sorted<Container> sorted(Container & container) { 
        return Sorted<Container>(container);
    }

}



#endif //#ifndef SORTED__HPP__
