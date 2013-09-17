cppitertools
============

range-based for loop add-ons inspired by the python builtins and itertools
library.


range
-----

Uses an underlying iterator to acheive the same effect of the python range
function.  `range` can be used in three different ways:

Only the stopping point is provided.  Prints 1 2 3 4 5 6 7 8 9
```c++
for (auto i : range(10)) {
    cout << i << '\n';
}
```

The start and stop are both provided.  Prints 10 11 12 13 14
```c++
for (auto i : range(10, 15)) {
    cout << i << '\n';
}
```

The start, stop, and step are all provided.  Prints 20 22 24 26 28
```c++
for (auto i : range(20, 30, 2)) {
    cout << i << '\n';
}
```

Negative values are allowed as well.  Prints 2 1 0 -1 -2
```c++
for (auto i : range(2, -3, -1)) {
    cout << i << '\n';
}
```


enumerate
---------

Can be used with any class with an iterator.  Continually "yields" containers
similar to pairs.  They are basic structs with a .index and a .element.  Usage
appears as:

```c++
vector<int> vec{2, 4, 6, 8};
for (auto e : enumerate(vec)) { 
    cout << e.index
         << ": "
         << e.element
         << '\n';
}
```


cycle
-----

Repeatedly produce all values of an iterable.  The loop will be infinite, so a
`break` is necessary to exit.

Prints 1 2 3 repeatedly until `some_condition` is true
```c++
vector<int> vec{1, 2, 3};
for (auto i : cycle(vec)) {
    cout << i << '\n';
    if (some_condition) {
        break;
    }
}
```


zip
---

Zips and aribitrary amount of ranges together into a new range. Iterating over this 
range yields an iterators that dereferences to a tuple of iterators. Terminates on 
the shortest range.

Example usage:
```c++
std::array<int,4> i{{1,2,3,4}};                                            
std::vector<float> f{1.2,1.4,12.3,4.5,9.9};                                
std::vector<std::string> s{"i","like","apples","alot","dude"};             
std::array<double,5> d{{1.2,1.2,1.2,1.2,1.2}};                             
std::cout << std::endl << "Variadic template zip iterator" << std::endl;
for (auto e : iter::zip(i,f,s,d)) {                                        
    std::cout << iter::zip_get<0>(e) << " "                                
        << iter::zip_get<1>(e) << " "                                      
        << iter::zip_get<2>(e) << " "                                      
        << iter::zip_get<3>(e) << std::endl;                               
    iter::zip_get<1>(e)=2.2f; //modify the float array                     
}
```

`iter::zip_get` is used to readably dereference the iterators yielded


chain
-----

This can chain any set of ranges together as long as they're iterators dereference to
the same type.

Example usage:
```c++
 std::vector<int> empty{};                                                 
 std::vector<int> vec1{1,2,3,4,5,6};                                       
 std::array<int,4> arr1{{7,8,9,10}};                                       
 std::cout << std::endl << "Chain iter test" << std::endl;                 
 for (auto i : iter::chain(empty,vec1,arr1)) {                             
     std::cout << i << std::endl;                                          
 }
 ```
 
 reverse
 -------
 
 Pretty much just uses the already existing reverse iterators of a container, good for
 use in range based for loops.
 
 Example usage:
 ```c++
std::cout << std::endl << "Reverse range test" << std::endl << std::endl;  
for (auto i : iter::reverse(a)) {                                          
    std::cout << i << std::endl;                                           
}
```
