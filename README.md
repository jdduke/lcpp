lcpp
===========================

lcpp (list comprehensions in C++) is a header-based C++ library that provides list construction
using simple and straightforward syntax patterned after set-builder notation.


Example
-------------

Generate all pairs of numbers between 0 and 9 where the sum of each pair is 10:

    std::array<int,10> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto pairs = (from(a, a), [](int x, int y) { return x + y == 10; })();    
    
Generate pair-wise sums for all pairs satisfying x < y and y between 0 and 9:

    std::array<int, 10> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto sums =  (std::plus<int>() | (from(a, a), std::less<int>()))(); 
    
Pythagorean triples with elements between 1 and 20:
    
    auto range = [](int i0, int n) -> std::vector<int> {
      std::vector<int> r;
      std::generate_n(std::back_inserter(r), n, [=]() mutable { return i0++; });
      return r;
    };
    auto r = range(1, 20);
    auto triples = (from(r, r, r), [](int x, int y, int z) { return x < y && y < z; },
                                   [](int x, int y, int z) { return x*x + y*y == z*z; })();

Documentation
-------------

Samples and tests can be built using projects/makefiles generated via CMake.  
Any compiler with proper C++11 support (variadics required) should work; tested with GCC 4.7.0 and MinGW.

Licensing
---------

MIT.
Please see the file called LICENSE.

Contributions
-------------

