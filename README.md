CPPItertools
============

range-based for loop add-ons inspired by the python builtins and itertools
library.

##### Table of Contents
[range](#range)<br />
[filter](#filter)<br />
[enumerate](#enumerate)<br />
[cycle](#cycle)<br />
[zip](#zip)<br />
[chain](#chain)<br />
[reverse](#reverse)<br />

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

filter
------
Called as `filter(predicate, iterable)`.  The predicate can be any callable.
`filter` will only yield values that are true under the predicate.

Prints values greater than 4:  5 6 7 8
```c++
vector<int> vec{1, 5, 6, 7, 3, 2, 8, 3, 2, 1};
for (auto i : filter([] (int i) { return i > 4; }, vec)) {
    cout << i <<'\n';
}

```

takewhile
---------
Yields elements from an iterable until the first element that is false under
the predicate is encountered.

Prints 1 2 3 4.  (5 is false under the predicate)
```c++
vector<int> ivec{1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1};
for (auto i : takewhile([] (int i) {return i < 5;}, ivec)) {
    cout << i << '\n';
}
```

dropwhile
---------
Yields all elements after and including the first element that is false under
the predicate.

Prints 5 6 7 1 2
```c++
vector<int> ivec{1, 2, 3, 4, 5, 6, 7, 1, 2};
for (auto i : dropwhile([] (int i) {return i < 5;}, ivec)) {
    cout << i << '\n';
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

Takes an arbitrary number of ranges of different types and efficiently iterates over 
them in parallel (so an iterator to each container is incremented simultaneously). 
When you dereference an iterator to "zipped" range you get a tuple of iterators to 
those containers.

Example usage:
```c++
array<int,4> i{{1,2,3,4}};                                            
vector<float> f{1.2,1.4,12.3,4.5,9.9};                                
vector<string> s{"i","like","apples","alot","dude"};             
array<double,5> d{{1.2,1.2,1.2,1.2,1.2}};                             

for (auto e : zip(i,f,s,d)) {                                        
    cout << zip_get<0>(e) << " "                                
         << zip_get<1>(e) << " "                                      
         << zip_get<2>(e) << " "                                      
         << zip_get<3>(e) << endl;                               
    zip_get<1>(e)=2.2f; // modify the float array                     
}
```

`iter::zip_get` is used to readably dereference the iterators yielded


chain
-----

This can chain any set of ranges together as long as their iterators
dereference to the same type.

```c++
vector<int> empty{};                                                 
vector<int> vec1{1,2,3,4,5,6};                                       
array<int,4> arr1{{7,8,9,10}};                                       

for (auto i : chain(empty,vec1,arr1)) {                             
    cout << i << endl;                                          
}
```

reverse
-------

Iterates over elements of a sequence in reverse order.

```c++
for (auto i : reverse(a)) {                                          
    cout << i << endl;                                           
}
```
