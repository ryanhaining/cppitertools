CPPItertools
============

range-based for loop add-ons inspired by the Python builtins and itertools
library.  Like itertools and the Python3 builtins, this library uses lazy
evaluation wherever possible.

*Note*: Everthing is inside the `iter` namespace.

##### Table of Contents
[range](#range)<br />
[enumerate](#enumerate)<br />
[zip](#zip)<br />
[imap](#imap)<br />
[filter](#filter)<br />
[filterfalse](#filterfalse)<br />
[unique_everseen](#unique_everseen)<br />
[unique_justseen](#unique_justseen)<br />
[takewhile](#takewhile)<br />
[dropwhile](#dropwhile)<br />
[cycle](#cycle)<br />
[groupby](#groupby)<br />
[accumulate](#accumulate)<br />
[compress](#compress)<br />
[chain](#chain)<br />
[chain.from\_iterable](#chainfrom_iterable)<br />
[reversed](#reversed)<br />
[slice](#slice)<br />
[sliding_window](#sliding_window)<br />
[grouper](#grouper)<br />

##### Combinatoric fuctions
[product](#product)<br />
[combinations](#combinations)<br />
[permutations](#permutations)<br />
[powerset](#powerset)<br />

range
-----

Uses an underlying iterator to acheive the same effect of the python range
function.  `range` can be used in three different ways:

Only the stopping point is provided.  Prints `0 1 2 3 4 5 6 7 8 9`
```c++
for (auto i : range(10)) {
    cout << i << '\n';
}
```

The start and stop are both provided.  Prints `10 11 12 13 14`
```c++
for (auto i : range(10, 15)) {
    cout << i << '\n';
}
```

The start, stop, and step are all provided.  Prints `20 22 24 26 28`
```c++
for (auto i : range(20, 30, 2)) {
    cout << i << '\n';
}
```

Negative values are allowed as well.  Prints `2 1 0 -1 -2`
```c++
for (auto i : range(2, -3, -1)) {
    cout << i << '\n';
}
```

In addition to normal integer range operations, doubles and
other numeric types are supported through the template

Prints: `5.0 5.5 6.0` ... `9.5`
```c++
for(auto i : range(5.0, 10.0, 0.5)) {
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

Prints values greater than 4:  `5 6 7 8`
```c++
vector<int> vec{1, 5, 4, 0, 6, 7, 3, 0, 2, 8, 3, 2, 1};
for (auto i : filter([] (int i) { return i > 4; }, vec)) {
    cout << i <<'\n';
}

```

If no predicate is passed, the elements themselves are tested for truth

Prints only non-zero values.
```c++
for(auto i : filter(vec)) {
    cout << i << '\n';
}
```

filterfalse
-----------
Similar to filter, but only prints values that are false under the predicate.

Prints values not greater than 4: `1 4 3 2 3 2 1 `
```c++
vector<int> vec{1, 5, 4, 0, 6, 7, 3, 0, 2, 8, 3, 2, 1};
for (auto i : filterfalse([] (int i) { return i > 4; }, vec)) {
    cout << i <<'\n';
}

```

If no predicate is passed, the elements themselves are tested for truth.

Prints only zero values.
```c++
for(auto i : filterfalse(vec)) {
    cout << i << '\n';
}
```
unique_everseen
---------------
This is a filter adaptor that only generates values that have never been seen 
before. For this algo to work your object must be specialized for `std::hash`
otherwise it will not be very efficient

Example Usage:
```c++
std::vector<int> v {1,2,3,4,3,2,1,5,6,7,7,8,9,8,9,6};                      
for (auto i : unique_everseen(v)) {                                        
    std::cout << i << " ";                                                 
}std::cout << std::endl; 
```

unique_justseen
--------------
Another filter adaptor that only prevents duplicates that are in a row, if the 
sequence is sorted it will work exactly the same as `unique_justseen`, in that 
case it will be better and more efficient to use.

Example Usage:
```c++
std::vector<int> v {1,1,1,2,2,4,4,5,6,7,8,8,8,8,9,9};                          
for (auto i : unique_justseen(v)) {                                            
    std::cout << i << " ";                                                     
}std::cout << std::endl; 
```

takewhile
---------
Yields elements from an iterable until the first element that is false under
the predicate is encountered.

Prints `1 2 3 4`.  (5 is false under the predicate)
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

Prints `5 6 7 1 2`
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

Prints `1 2 3` repeatedly until `some_condition` is true
```c++
vector<int> vec{1, 2, 3};
for (auto i : cycle(vec)) {
    cout << i << '\n';
    if (some_condition) {
        break;
    }
}
```

groupby
-------
Separate an iterable into groups sharing a common key.  The following example
creates a new group whenever a string of a different length is encountered.
```c++
vector<string> vec = {
    "hi", "ab", "ho",
    "abc", "def",
    "abcde", "efghi"
};

for (auto gb : groupby(vec, [] (const string &s) {return s.length(); })) {
    cout << "key: " << gb.first << '\n';
    cout << "content: ";
    for (auto s : gb.second) {
        cout << s << "  ";
    }
    cout << '\n';
}
```
*Note*: Just like Python's `itertools.groupby`, this doesn't do any sorting.
It just iterates through, making a new group each time there is a key change.
Thus, if the the group is unsorted, the same key may appear multiple times.

accumulate
-------
Differs from `std::accumulate` (which in my humble opinion should be named 
`std::reduce` or `std::foldl`).  It is similar to a functional reduce where one 
can see all of the intermediate results.  By default, it keeps a running sum.
Prints: `1 3 6 10 15`
```c++
for (auto i : accumulate(range(1, 6))) {
    cout << i << '\n';
}
```
A second, optional argument may provide an alternative binary function 
to compute results.  The following example multiplies the numbers, rather
than adding them.
Prints: `1 2 6 24 120`

```c++
for (auto i : accumulate(range(1, 6), std::multiplies<int>{})) {
    cout << i << '\n';
}
```

Note: The intermediate result type must support default construction
and assignment.

zip
---

Takes an arbitrary number of ranges of different types and efficiently iterates over 
them in parallel (so an iterator to each container is incremented simultaneously). 
When you dereference an iterator to "zipped" range you get a tuple of whatever elements 
the iterators were holding.

Example usage:
```c++
array<int,4> i{{1,2,3,4}};                                            
vector<float> f{1.2,1.4,12.3,4.5,9.9};                                
vector<string> s{"i","like","apples","alot","dude"};             
array<double,5> d{{1.2,1.2,1.2,1.2,1.2}};                             

for (auto e : zip(i,f,s,d)) {                                        
    cout << std::get<0>(e) << ' '                                
         << std::get<1>(e) << ' '                                      
         << std::get<2>(e) << ' '                                      
         << std::get<3>(e) << '\n';                               
    std::get<1>(e)=2.2f; // modify the float array                     
}
```


a `zip_longest` also exists where the range terminates on the longest
range instead of the shortest. because of that you have to return a
`boost::optional<T>` where `T` is whatever type the iterator dereferenced
to (`std::optional` when it is released, if ever)


imap
----

Takes a function and one or more iterables.  The number of iterables must
match the number of arguments to the function.  Applies the function to
each element (or elements) in the iterable(s).  Terminates on the shortest
sequence.

Prints the squares of the numbers in vec: `1 4 9 16 25`
```c++
vector<int> vec{1, 2, 3, 4, 5};
for (auto i : imap([] (int x) {return x * x;}, vec)) {
    cout << i << '\n';
}
```

With more than one sequence, the below adds corresponding elements from
each vector together, printing `11 23 35 47 59 71`
```c++
vector<int> vec1{1, 3, 5, 7, 9, 11};
vector<int> vec2{10, 20, 30, 40, 50, 60};
for (auto i : imap([] (int x, int y) { return x + y; }, vec1, vec2)) {
    cout << i << '\n';
}
```

*Note*: The name `imap` is chosen to prevent confusion/collision with 
`std::map`, and because it is more related to `itertools.imap` than
the python builtin `map`.


compress
--------

Yields only the values corresponding to true in the selectors iterable.
Terminates on the shortest sequence.

Prints `2 6`
```c++
vector<int> ivec{1, 2, 3, 4, 5, 6};
vector<bool> bvec{false, true, false, false, false, true};
for (auto i : compress(ivec, bvec) {
    cout << i << '\n';
}
```

chain
-----

This can chain any set of ranges together as long as their iterators
dereference to the same type.

```c++
vector<int> empty{};                                                 
vector<int> vec1{1,2,3,4,5,6};                                       
array<int,4> arr1{{7,8,9,10}};                                       

for (auto i : chain(empty,vec1,arr1)) {                             
    cout << i << '\n';                                          
}
```

chain.from_iterable
-------------------

Similar to chain, but rather than taking a variadic number of iterables,
it takes an iterable of iterables and chains the contained iterables together.
A simple example is shown below using a vector of vectors to represent
a 2d ragged array, and prints it in row-major order.
```c++
vector<vector<int>> matrix = {
    {1, 2, 3},
    {4, 5},
    {6, 8, 9, 10, 11, 12}
};

for (auto i : chain.from_iterable(matrix)) {
    cout << i << '\n';
}
```

reversed
-------

Iterates over elements of a sequence in reverse order.

```c++
for (auto i : reversed(a)) {                                          
    cout << i << '\n';                                           
}
```

slice
-----

Returns selected elements from a range, parameters are start, stop and step.
the range returned is [start,stop) where you only take every step element

This outputs `0 3 6 9 12`
```c++
vector<int> a{0,1,2,3,4,5,6,7,8,9,10,11,12,13};
for (auto i : slice(a,0,15,3)) {
    cout << i << '\n';
}
```

sliding_window
-------------

Takes a section from a range and increments the whole section.  

Example:
`[1, 2, 3, 4, 5, 6, 7, 8, 9]`  

take a section of size 4, output is:
```
1 2 3 4 
2 3 4 5 
3 4 5 6 
4 5 6 7 
5 6 7 8 
6 7 8 9 
```

Example Usage:
```c++
std::vector<int> v = {1,2,3,4,5,6,7,8,9};                                      
for (auto sec : sliding_window(v,4)) {                                         
    for (auto i : sec) {                                                       
        std::cout << i << " ";                                                 
        i.get() = 90; 
        //has to be accessed with get if you want to store references
        //because it is stored in a reference_wrapper (std::vector 
        //cannot hold references)
    }                                                                          
    std::cout << std::endl;                                                    
}
```  
grouper
------

grouper is very similar to sliding window, except instead of the 
section sliding by only 1 it goes the length of the full section.

Example usage:
```c++
std::vector<int> v {1,2,3,4,5,6,7,8,9};                                        
for (auto sec : grouper(v,4)) 
//each section will have 4 elements
//except the last one may be cut short
{
    for (auto i : sec) {                                                       
        std::cout << i << " ";                                                 
        i.get() *= 2;                                                          
    }                                                                          
    std::cout << std::endl;                                                    
}  
```

product
------

Generates the cartesian project of the given ranges put together  

Example usage: 
```c++
std::vector<int> v1{1,2,3};                                                    
std::vector<int> v2{7,8};                                                      
std::vector<std::string> v3{"the","cat"};                                      
std::vector<std::string> v4{"hi","what","up","dude"}; 
for (auto t : product(v1,v2,v3,v4)) {                                          
    std::cout << std::get<0>(t) << ", "                                        
        << std::get<1>(t) << ", "                                              
        << std::get<2>(t) << ", "                                              
        << std::get<3>(t) << std::endl;                                        
} 
```

combinations
-----------

Generates n length unique sequences of the input range, there is also a
combinations_with_replacement

Example usage:
```c++
std::vector<int> v = {1,2,3,4,5};                                              
for (auto i : combinations(v,3)) {                                             
    //std::cout << i << std::endl;                                             
    for (auto j : i ) std::cout << j << " ";                                   
    std::cout<<std::endl;                                                      
}
```


permutations
-----------

Generates all the permutations of a range using `std::next_permutation`  

Example usage:
```c++
std::vector<int> v = {1,2,3,4,5};                                              
for (auto vec : permutations(v)) {                                             
    for (auto i : vec) {                                                       
        std::cout << i << " ";                                                 
    }                                                                       
    std::cout << std::endl;                                                 
} 
```

powerset
-------

Generates every possible subset of a set, never run it since it runs in 𝚯(2^n).

Example usage:
```c++
std::vector<int> vec {1,2,3,4,5,6,7,8,9};                                      
for (auto v : powerset(vec)) {                                                 
    for (auto i : v) std::cout << i << " ";                                    
    std::cout << std::endl;                                                    
}
```

  
