#include <iterator>
namespace iter {
    template <typename Container>
        auto slice(
                Container & container,
                typename std::iterator_traits<decltype(container.begin())>::difference_type begin,
                typename std::iterator_traits<decltype(container.begin())>::difference_type end
                ) -> iterator_range<decltype(container.begin())>
        {
            return iterator_range<decltype(container.begin())>(
                    container.begin()+begin,
                    container.begin()+end);
        }
};
