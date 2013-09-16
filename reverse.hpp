namespace iter {
    template <typename Container>
       auto reverse(Container & container) -> iterator_range<decltype(container.rbegin())>
       {
           return 
               iterator_range<decltype(container.rbegin())>(container.rbegin(),container.rend());
       }
}
