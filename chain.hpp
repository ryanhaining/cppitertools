namespace iter {
   //chain operates only as a range, it has no iterator
   //this is because each iterator would have to be aware of
   //it's end for this to work
   template <typename ... Containers>
       struct chain_iter;
   template <typename Container>
       struct chain_iter<Container> {
           using Iterator = decltype(((Container*)nullptr)->begin());
           Iterator begin;
           Iterator end;//never really used but kept it for consistency
           chain_iter(Container & container, bool is_end=false) :
               begin(container.begin()),end(container.end()) {
                   //std::cout << *begin<<"hi" << std::endl;
                   if(is_end) begin = container.end();
           }
           chain_iter & operator++()
           {
               ++begin;
               return *this;
           }
           auto operator*()->decltype(*begin)
           {
               return *begin;
           }
           bool operator!=(const chain_iter & rhs) {
               return this->begin != rhs.begin;
           }
       };
   template <typename Container, typename ... Containers>
       struct chain_iter<Container,Containers...>
       {
           using Iterator = decltype(((Container*)nullptr)->begin());
           Iterator begin;
           Iterator end;
           bool end_reached = false;
           chain_iter<Containers...> next_iter;
           chain_iter(Container & container, Containers& ... rest, bool is_end=false) :
               begin(container.begin()),
               end(container.end()),
               next_iter(rest...,is_end) {
                   //std::cout << *begin << std::endl;
                   if(is_end)
                       begin = container.end();
               }
           chain_iter & operator++()
           {
               if (begin == end) {
                   /*
                   //std::cout << "fuck" << std::endl;
                   if(end_reached) {
                       //std::cout << "++next_iter" << std::endl;
                       ++next_iter;
                   }
                   else {
                       //std::cout << "++" << std::endl;
                       end_reached = true;
                   }
                   */
                   ++next_iter;
               }
               else {
                   ++begin;
               }
               return *this;               
           }
           auto operator*()->decltype(*begin)
           {
               //std::cout << *begin << std::endl;
               if (begin == end) {
                   //std::cout << "*" << std::endl;
                   return *next_iter;
               }
               else {
                   return *begin;
               }
           }   
           bool operator !=(const chain_iter & rhs) {
               if (begin == end) {
                   //std::cout << "!=" << std::endl;
                   return this->next_iter != rhs.next_iter;
               }
               else
                   return this->begin != rhs.begin;
           }
        };
   template <typename ... Containers>
       iterator_range<chain_iter<Containers...>> chain(Containers& ... containers)
       {
           auto begin = 
               chain_iter<Containers...>(containers...);
           auto end =
               chain_iter<Containers...>(containers...,true);
           return 
               iterator_range<chain_iter<Containers...>>(begin,end);
       }
};
