/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <array>
#include <iostream>

#include "lcpp.h"

int main(int argc, char* argv[]) {

  using namespace lc;

  ///////////////////////////////////////////////////////////////////////////
  // arrays, vectors and other templated containers are all supported

  const std::array<int, 10> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  const std::array<int, 10> b = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};


  ///////////////////////////////////////////////////////////////////////////
  // Multiple sources, single condition

  std::cout << "x < y: " << std::endl
            << (from(a, b) , [](int x, int y) { return x < y; })()
            << std::endl << std::endl;


  ///////////////////////////////////////////////////////////////////////////
  // Multiple sources, multiple conditions

  std::cout << "(x + y < z) && (x > y): " << std::endl
            << (from(a, a, a) , [](int x, int y, int z) { return x + y < z; }
                              , [](int x, int y, int z) { return x > y; })()
            << std::endl<< std::endl;


  ///////////////////////////////////////////////////////////////////////////
  // Iterators

  auto list = (from(a, b), [](int x, int y) { return x == y; });

  std::cout << "(x == y) using iterators: " << std::endl;
  for(auto it = std::begin(list); it != std::end(list); ++it) {
    std::cout << *it;
  }

  std::cout << std::endl << std::endl;


///////////////////////////////////////////////////////////////////////////
  // Multiple sources, multiple conditions and a selecting transform

  auto mult_triple = [](int x, int y, int z) { return x * y * z; };
  std::cout << "(x*y*z) where (x+y+z==10):" << std::endl
            << (mult_triple | (from(a, a, a), [](int x, int y, int z) { return x + y + z == 10; }))()
            << std::endl << std::endl;


  ///////////////////////////////////////////////////////////////////////////
  // Custom (non-tuple result) transform

  auto tf   = [](int x, int y) { return x + y;  };
  auto cond = [](int x, int y) { return x == y; };
  std::cout << "Untransformed: " << (from(a, b), cond)() << std::endl;
  std::cout << "Transformed:   " << (tf | (from(a, b), cond))() << std::endl;

  ///////////////////////////////////////////////////////////////////////////
  // Pythagorean triples

  auto range = [](int i0, int n) -> std::vector<int> {
    std::vector<int> r;
    std::generate_n(std::back_inserter(r), n, [=]() mutable { return i0++; });
    return r;
  };
  auto r = range(1, 20);
  std::cout << "PythagoreanTriples: "
            << (from(r, r, r), [](int x, int y, int z) { return x < y && y < z; },
                               [](int x, int y, int z) { return x*x + y*y == z*z; })() << std::endl;

}
