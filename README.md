CPPItertools
============
Range-based for loop add-ons inspired by the Python builtins and itertools
library.  Like itertools and the Python3 builtins, this library uses lazy
evaluation wherever possible.

*Note*: Everthing is inside the `iter` namespace.

Follow [@cppitertools](https://twitter.com/cppitertools) for updates.

#### Table of Contents
[range](#range)<br />
[enumerate](#enumerate)<br />
[zip](#zip)<br />
[zip\_longest](#zip)<br />
[imap](#imap)<br />
[filter](#filter)<br />
[filterfalse](#filterfalse)<br />
[unique\_everseen](#unique_everseen)<br />
[unique\_justseen](#unique_justseen)<br />
[takewhile](#takewhile)<br />
[dropwhile](#dropwhile)<br />
[cycle](#cycle)<br />
[repeat](#repeat)<br />
[count](#count)<br />
[groupby](#groupby)<br />
[starmap](#starmap)<br />
[accumulate](#accumulate)<br />
[compress](#compress)<br />
[sorted](#sorted)<br />
[chain](#chain)<br />
[chain.from\_iterable](#chainfrom_iterable)<br />
[reversed](#reversed)<br />
[slice](#slice)<br />
[sliding\_window](#sliding_window)<br />
[chunked](#chunked)<br />

##### Combinatoric fuctions
[product](#product)<br />
[combinations](#combinations)<br />
[combinations\_with\_replacement](#combinations_with_replacement)<br />
[permutations](#permutations)<br />
[powerset](#powerset)<br />

#### Requirements
This library is **header-only** and relies only on the C++ standard
library. The only exception is `zip_longest` which uses `boost::optional`.
`#include <cppitertools/itertools.hpp>` will include all of the provided
tools except for `zip_longest` which must be included separately.  You may
also include individual pieces with the relevant header
(`#include <cppitertools/enumerate.hpp>` for example).


### Running tests
You may use either `scons` or `bazel` to build the tests. `scons` seems
to work better with viewing the test output, but the same `bazel` command
can be run from any directory.

To run tests with scons you must be within the `test` directory

```sh
test$ # build and run all tests
test$ scons
test$ ./test_all
test$ # build and run a specific test
test$ scons test_enumerate
test$ ./test_enumerate
test$ valgrind ./test_enumerate
```

`bazel` absolute commands can be run from any directory inside the project

```sh
$ bazel test //test:all # runs all tests
$ bazel test //test:test_enumerate # runs a specific test
```

#### Requirements of passed objects
Most itertools will work with iterables using InputIterators and not copy
or move any underlying elements.  The itertools that need ForwardIterators or
have additional requirements are noted in this document. However, the cases
should be fairly obvious: any time an element needs to appear multiple times
(as in `combinations` or `cycle`) or be looked at more than once (specifically,
`sorted`).
This library takes every effort to rely on as little as possible from the
underlying iterables, but if anything noteworthy is needed it is described
in this document.

#### Guarantees of implementations
By implementations I mean the objects returned by the API's functions. All of
the implementation classes are move-constructible, not copy-constructible,
not assignable. All iterators that work over another iterable are tagged
as InputIterators and behave as such.

#### Feedback
If you find anything not working as you expect, not compiling when you believe
it should, a divergence from the python itertools behavior, or any sort of
error, please let me know. The preferable means would be to open an issue on
github. If you want to talk about an issue that you don't feel would be
appropriate as a github issue (or you just don't want to open one),
You can email me directly with whatever code you have that describes the
problem, I've been pretty responsive in the past. If I believe you are
"misusing" the library I'll try to put the blame on myself for being unclear
in this document and take the steps to clarify it.  So please, contact me with
any concerns, I'm open to feedback.

#### How (not) to use this library
The library functions create and return objects that are properly templated on
the iterable they are passed. These exact names of these types or
precisely how they are templated is unspecified, you should rely on the
functions described in this document.
If you plan to use these functions in very simple, straight forward means as in
the examples on this page, then you will be fine. If you feel like you need to
open the header files, then I've probably under-described something, let me
know.

#### Handling of rvalues vs lvalues
The rules are pretty simple, and the library can be largely used without
knowledge of them.
Let's take an example
```c++
std::vector<int> vec{2,4,6,8};
for (auto&& p : enumerate(vec)) { /* ... */ }
```
In this case, `enumerate` will return an object that has bound a reference to
`vec`. No copies are produced here, neither of `vec` nor of the elements it
holds.

If an rvalue was passed to enumerate, binding a reference would be unsafe.
Consider:
```c++
for (auto&& p : enumerate(std::vector<int>{2,4,6,8})) { /* ... */ }
```
Instead, `enumerate` will return an object that has the temporary *moved* into
it.  That is, the returned object will contain a `std::vector<int>` rather than
just a reference to one. This may seem like a contrived example, but it matters
when `enumerate` is passed the result of a function call like `enumerate(f())`,
or, more obviously, something like `enumerate(zip(a, b))`.  The object returned
from `zip` must be moved into the `enumerate` object. As a more specific
result, itertools can be mixed and nested.



#### Pipe syntax
Wherever it makes sense I've implemented the "pipe" operator that has become
common in similar libraries. When the syntax is available it is done by pulling
out the iterable from the call and placing it before the tool. For example:

```c++
filter(pred, seq);  // regular call
seq | filter(pred);  // pipe-style
enumerate(seq);  // regular call
seq | enumerate;  // pipe-style.
```

The following tools support pipe. The remaining I left out because although
some of them have multiple reasonable versions, it wasn't obvious to me how I
would expect them to behave:

- accumulate
- chain.from\_iterable
- chunked
- combinations
- combinations\_with\_replacement
- cycle
- dropwhile
- enumerate
- filter
- filterfalse
- groupby
- imap
- permutations
- powerset
- reversed
- slice
- sliding\_window
- sorted
- starmap
- takewhile
- unique\_everseen
- unique\_justseen

I don't personally care for the piping style, but it seemed to be desired by
the users.


range
-----

Uses an underlying iterator to achieve the same effect of the python range
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

A step size of 0 results in an empty range (Python's raises an exception).
The following prints nothing
```c++
for (auto i : range(0, 10, 0)) {
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

*Implementation Note*: Typical ranges have their current value incremented by
the step size repeatedly (`value += step`). Floating point range value are
recomputed at each step to avoid accumulating floating point inaccuracies
(`value = start + (step * steps_taken`). The result of the latter is a bit
slower but more accurate.

enumerate
---------

Continually "yields" containers similar to pairs.  They are basic structs with a
.index and a .element, and also work with structured binding declarations.
Usage appears as:

```c++
vector<int> vec{2, 4, 6, 8};
for (auto&& [i, e] : enumerate(vec)) {
    cout << i << ": " << e << '\n';
}
```

filter
------
Called as `filter(predicate, iterable)`.  The predicate can be any callable.
`filter` will only yield values that are true under the predicate.

Prints values greater than 4:  `5 6 7 8`
```c++
vector<int> vec{1, 5, 4, 0, 6, 7, 3, 0, 2, 8, 3, 2, 1};
for (auto&& i : filter([] (int i) { return i > 4; }, vec)) {
    cout << i <<'\n';
}

```

If no predicate is passed, the elements themselves are tested for truth

Prints only non-zero values.
```c++
for(auto&& i : filter(vec)) {
    cout << i << '\n';
}
```

filterfalse
-----------
Similar to filter, but only prints values that are false under the predicate.

Prints values not greater than 4: `1 4 3 2 3 2 1 `
```c++
vector<int> vec{1, 5, 4, 0, 6, 7, 3, 0, 2, 8, 3, 2, 1};
for (auto&& i : filterfalse([] (int i) { return i > 4; }, vec)) {
    cout << i <<'\n';
}

```

If no predicate is passed, the elements themselves are tested for truth.

Prints only zero values.
```c++
for(auto&& i : filterfalse(vec)) {
    cout << i << '\n';
}

```
unique\_everseen
---------------
*Additional Requirements*: Underlying values must be copy-constructible.

This is a filter adaptor that only generates values that have never been seen
before. For this to work your object must be specialized for `std::hash`.

Prints `1 2 3 4 5 6 7 8 9`
```c++
vector<int> v {1,2,3,4,3,2,1,5,6,7,7,8,9,8,9,6};
for (auto&& i : unique_everseen(v)) {
    cout << i << ' ';
}
```

unique\_justseen
--------------
Another filter adaptor that only omits consecutive duplicates.

Prints `1 2 3 4 3 2 1`
Example Usage:
```c++
vector<int> v {1,1,1,2,2,3,3,3,4,3,2,1,1,1};
for (auto&& i : unique_justseen(v)) {
    cout << i << ' ';
}
```

takewhile
---------
Yields elements from an iterable until the first element that is false under
the predicate is encountered.

Prints `1 2 3 4`.  (5 is false under the predicate)
```c++
vector<int> ivec{1, 2, 3, 4, 5, 6, 7, 6, 5, 4, 3, 2, 1};
for (auto&& i : takewhile([] (int i) {return i < 5;}, ivec)) {
    cout << i << '\n';
}
```

dropwhile
---------
Yields all elements after and including the first element that is true under
the predicate.

Prints `5 6 7 1 2`
```c++
vector<int> ivec{1, 2, 3, 4, 5, 6, 7, 1, 2};
for (auto&& i : dropwhile([] (int i) {return i < 5;}, ivec)) {
    cout << i << '\n';
}
```

cycle
-----
*Additional Requirements*: Input must have a ForwardIterator


Repeatedly produces all values of an iterable.  The loop will be infinite, so a
`break` or other control flow structure is necessary to exit.

Prints `1 2 3` repeatedly until `some_condition` is true
```c++
vector<int> vec{1, 2, 3};
for (auto&& i : cycle(vec)) {
    cout << i << '\n';
    if (some_condition) {
        break;
    }
}
```

repeat
------
Repeatedly produces a single argument forever, or a given number of times.
`repeat` will bind a reference when passed an lvalue and move when given
an rvalue.  It will then yield a reference to the same item until completion.

The below prints `1` five times.
```c++
for (auto&& e : repeat(1, 5)) {
    cout << e << '\n';
}
```

The below prints `2` forever
```c++
for (auto&& e : repeat(2)) {
    cout << e << '\n';
}
```

count
-----
Effectively a `range` without a stopping point.<br />
`count()` with no arguments will start counting from 0 with a positive
step of 1.<br />
`count(i)` will start counting from `i` with a positive step of 1.<br />
`count(i, st)` will start counting from `i` with a step of `st`.

*Technical limitations*: Unlike Python which can use its long integer
types when needed, count() would eventually exceed the
maximum possible value for its type (or minimum with a negative step).
`count` is actually implemented as a `range` with the stopping point
being the `std::numeric_limits<T>::max()` for the integral type (`long`
by default)

The below will print `0 1 2` ... etc
```c++
for (auto&& i : count()) {
    cout << i << '\n';
}
```

groupby
-------
*Additional Requirements*: If the Input's iterator's `operator*()` returns
a reference, the reference must remain valid after the iterator is incremented.
Roughly equivalent to requiring the Input have a ForwardIterator.

Separate an iterable into groups sharing a common key.  The following example
creates a new group whenever a string of a different length is encountered.
```c++
vector<string> vec = {
    "hi", "ab", "ho",
    "abc", "def",
    "abcde", "efghi"
};

for (auto&& gb : groupby(vec, [] (const string &s) {return s.length(); })) {
    cout << "key: " << gb.first << '\n';
    cout << "content: ";
    for (auto&& s : gb.second) {
        cout << s << "  ";
    }
    cout << '\n';
}
```
*Note*: Just like Python's `itertools.groupby`, this doesn't do any sorting.
It just iterates through, making a new group each time there is a key change.
Thus, if the group is unsorted, the same key may appear multiple times.

starmap
-------

Takes a sequence of tuple-like objects (anything that works with `std::get`)
and unpacks each object into individual arguments for each function call.
The below example takes a `vector` of `pairs` of ints, and passes them
to a function expecting two ints, with the elements of the `pair` being
the first and second arguments to the function.

```c++
vector<pair<int, int>> v = {{2, 3}, {5, 2}, {3, 4}}; // {base, exponent}
for (auto&& i : starmap([](int b, int e){return pow(b, e);}, v)) {
    // ...
}
```

`starmap` can also work over a tuple-like object of tuple-like objects even
when the contained objects are different as long as the functor works with
multiple types of calls.  For example, a `Callable` struct with overloads
for its `operator()` will work as long as all overloads have the same
return type

```c++
struct Callable {
    int operator()(int i) const;
    int operator()(int i, char c) const;
    int operator()(double d, int i, char c) const;
};
```

This will work with a tuple of mixed types

```c++
auto t = make_tuple(
        make_tuple(5), // first form
        make_pair(3, 'c'), // second
        make_tuple(1.0, 1, '1')); // third
for (auto&& i : starmap(Callable{}, t)) {
    // ...
}
```

accumulate
-------
*Additional Requirements*: Type return from functor (with reference removed)
must be assignable.

Differs from `std::accumulate` (which in my humble opinion should be named
`std::reduce` or `std::foldl`).  It is similar to a functional reduce where one
can see all of the intermediate results.  By default, it keeps a running sum.
Prints: `1 3 6 10 15`
```c++
for (auto&& i : accumulate(range(1, 6))) {
    cout << i << '\n';
}
```
A second, optional argument may provide an alternative binary function
to compute results.  The following example multiplies the numbers, rather
than adding them.
Prints: `1 2 6 24 120`

```c++
for (auto&& i : accumulate(range(1, 6), std::multiplies<int>{})) {
    cout << i << '\n';
}
```

Note: The intermediate result type must support default construction
and assignment.

zip
---
Takes an arbitrary number of ranges of different types and efficiently iterates
over them in parallel (so an iterator to each container is incremented
simultaneously).  When you dereference an iterator to "zipped" range you get a
tuple of the elements the iterators were holding.

Example usage:
```c++
array<int,4> iseq{{1,2,3,4}};
vector<float> fseq{1.2,1.4,12.3,4.5,9.9};
vector<string> sseq{"i","like","apples","a lot","dude"};
array<double,5> dseq{{1.2,1.2,1.2,1.2,1.2}};

for (auto&& [i, f, s, d] : zip(iseq, fseq, sseq, dseq)) {
    cout << i << ' ' << f << ' ' << s << ' ' << d << '\n';
    f = 2.2f; // modifies the underlying 'fseq' sequence
}
```

zip\_longest
-----------
Terminates on the longest sequence instead of the shortest.
Repeatedly yields a tuple of `boost::optional<T>`s where `T` is the type
yielded by the sequences' respective iterators.  Because of its boost
dependency, `zip_longest` is not in `itertools.hpp` and must be included
separately.
The following loop prints either "Just <item>" or "Nothing" for each
element in each tuple yielded.

```c++
vector<int> v1 = {0, 1, 2, 3};
vector<int> v2 = {10, 11};
for (auto&& [x, y] : zip_longest(v1, v2)) {
    cout << '{';
    if (x) {
        cout << "Just " << *x;
    } else {
        cout << "Nothing";
    }
    cout << ", ";
    if (y) {
        cout << "Just " << *y;
    } else {
        cout << "Nothing";
    }
    cout << "}\n";
}
```

The output is:
```
{Just 0, Just 10}
{Just 1, Just 11}
{Just 2, Nothing}
{Just 3, Nothing}
```

imap
----

Takes a function and one or more iterables.  The number of iterables must
match the number of arguments to the function.  Applies the function to
each element (or elements) in the iterable(s).  Terminates on the shortest
sequence.

Prints the squares of the numbers in vec: `1 4 9 16 25`
```c++
vector<int> vec{1, 2, 3, 4, 5};
for (auto&& i : imap([] (int x) {return x * x;}, vec)) {
    cout << i << '\n';
}
```

With more than one sequence, the below adds corresponding elements from
each vector together, printing `11 23 35 47 59 71`
```c++
vector<int> vec1{1, 3, 5, 7, 9, 11};
vector<int> vec2{10, 20, 30, 40, 50, 60};
for (auto&& i : imap([] (int x, int y) { return x + y; }, vec1, vec2)) {
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
for (auto&& i : compress(ivec, bvec) {
    cout << i << '\n';
}
```

sorted
------
*Additional Requirements*: Input must have a ForwardIterator

Allows iteration over a sequence in sorted order. `sorted` does
**not** produce a new sequence, copy elements, or modify the original
sequence.  It only provides a way to iterate over existing elements.
`sorted` also takes an optional second
[comparator](http://en.cppreference.com/w/cpp/concept/Compare)
argument.  If not provided, defaults to `std::less`. <br />
Iterables passed to sorted are required to have an iterator with
an `operator*() const` member.

The below outputs `0 1 2 3 4`.

```c++
unordered_set<int> nums{4, 0, 2, 1, 3};
for (auto&& i : sorted(nums)) {
    cout << i << '\n';
}
```

chain
-----
*Additional Requirements*: The underlying iterators of all containers'
`operator*` must have the *exact* same type

This can chain any set of ranges together as long as their iterators
dereference to the same type.

```c++
vector<int> empty{};
vector<int> vec1{1,2,3,4,5,6};
array<int,4> arr1{{7,8,9,10}};

for (auto&& i : chain(empty,vec1,arr1)) {
    cout << i << '\n';
}
```

chain.from\_iterable
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

for (auto&& i : chain.from_iterable(matrix)) {
    cout << i << '\n';
}
```

reversed
-------
*Additional Requirements*: Input must be compatible with `std::rbegin()` and
`std::rend()`

Iterates over elements of a sequence in reverse order.

```c++
for (auto&& i : reversed(a)) {
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
for (auto&& i : slice(a,0,15,3)) {
    cout << i << '\n';
}
```

sliding\_window
-------------
*Additional Requirements*: Input must have a ForwardIterator

Takes a section from a range and increments the whole section.  If the
window size is larger than the length of the input, the `sliding_window` will
yield nothing (begin == end).

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
vector<int> v = {1,2,3,4,5,6,7,8,9};
for (auto&& sec : sliding_window(v,4)) {
    for (auto&& i : sec) {
        cout << i << ' ';
        i.get() = 90;
    }
    cout << '\n';
}
```
chunked
------

chunked will yield subsequent chunkes of an iterable in blocks of a specified
size. The final chunk may be shorter than the rest if the chunk size given
does not evenly divide the length of the iterable

Example usage:
```c++
vector<int> v {1,2,3,4,5,6,7,8,9};
for (auto&& sec : chunked(v,4)) {
    for (auto&& i : sec) {
        cout << i << ' ';
    }
    cout << '\n';
}
```

The above prints:
```
1 2 3 4
5 6 7 8
9
```

product
------
*Additional Requirements*: Input must have a ForwardIterator

Generates the cartesian product of the given ranges put together.

Example usage:
```c++
vector<int> v1{1,2,3};
vector<int> v2{7,8};
vector<string> v3{"the","cat"};
vector<string> v4{"hi","what's","up","dude"};
for (auto&& [a, b, c, d] : product(v1,v2,v3,v4)) {
    cout << a << ", " << b << ", " << c << ", " << d << '\n';
}
```

Product also accepts a "repeat" as a template argument. Currently this is the only way to do repeats. **If you are reading this and need `product(seq, 3)` instead of `product<3>(seq)` please open an issue**.

Example usage:
```c++
std::string s = "abc";
// equivalent of product(s, s, s);
for (auto&& t : product<3>(s)) {
   // ...
}
```

combinations
-----------
*Additional Requirements*: Input must have a ForwardIterator

Generates n length unique sequences of the input range.

Example usage:
```c++
vector<int> v = {1,2,3,4,5};
for (auto&& i : combinations(v,3)) {
    for (auto&& j : i ) cout << j << " ";
    cout << '\n';
}
```

combinations\_with\_replacement
-----------------------------
*Additional Requirements*: Input must have a ForwardIterator

Like combinations, but with replacement of each element.  The
below is printed by the loop that follows:
```
{A, A}
{A, B}
{A, C}
{B, B}
{B, C}
{C, C}
```
```c++
for (auto&& v : combinations_with_replacement(s, 2)) {
    cout << '{' << v[0] << ", " << v[1] << "}\n";
}
```

permutations
-----------
*Additional Requirements*: Input must have a ForwardIterator.  Iterator must
have an `operator*() const`.

Generates all the permutations of a range using `std::next_permutation`.

Example usage:
```c++
vector<int> v = {1,2,3,4,5};
for (auto&& vec : permutations(v)) {
    for (auto&& i : vec) {
        cout << i << ' ';
    }
    cout << '\n';
}
```

powerset
-------
*Additional Requirements*: Input must have a ForwardIterator

Generates every possible subset of a set, runs in O(2^n).

Example usage:
```c++
vector<int> vec {1,2,3,4,5,6,7,8,9};
for (auto&& v : powerset(vec)) {
    for (auto&& i : v) {
        cout << i << " ";
    }
    cout << '\n';
}
```
