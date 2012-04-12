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
  using namespace std;

  ///////////////////////////////////////////////////////////////////////////
  // arrays, vectors and other templated containers are all supported

  const array<int, 10> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  const array<int, 10> b = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};


  ///////////////////////////////////////////////////////////////////////////
  // Multiple sources, single condition

  cout << "x < y: " << endl
            << (from(a, b) , [](int x, int y) { return x < y; })()
            << endl << endl;


  ///////////////////////////////////////////////////////////////////////////
  // Multiple sources, multiple conditions

  cout << "(x + y < z) && (x > y): " << endl
            << (from(a, a, a) , [](int x, int y, int z) { return x + y < z; }
                              , [](int x, int y, int z) { return x > y; })()
            << endl<< endl;


  ///////////////////////////////////////////////////////////////////////////
  // Iterators

  auto set = (from(a, b), [](int x, int y) { return x == y; });

  cout << "(x == y) using iterators: " << endl;
  for (const auto& s : set)
    cout << s;

  cout << endl << endl;


///////////////////////////////////////////////////////////////////////////
  // Multiple sources, multiple conditions and a selecting transform

  auto mult_triple = [](int x, int y, int z) { return x * y * z; };
  cout << "(x*y*z) where (x+y+z==10):" << endl
            << (mult_triple | (from(a, a, a), [](int x, int y, int z) { return x + y + z == 10; }))()
            << endl << endl;


  ///////////////////////////////////////////////////////////////////////////
  // Custom (non-tuple result) transform

  auto tf   = [](int x, int y) { return x + y;  };
  auto cond = [](int x, int y) { return x == y; };
  cout << "Untransformed: " <<       (from(a, b), cond) () << endl;
  cout << "Transformed:   " << (tf | (from(a, b), cond))() << endl;

  ///////////////////////////////////////////////////////////////////////////
  // Pythagorean triples

  vector<int> r(20);
  iota(begin(r), end(r), 1);
  cout << "PythagoreanTriples: "
            << (from(r, r, r), [](int x, int y, int z) { return x < y && y < z; },
                               [](int x, int y, int z) { return x*x + y*y == z*z; })() << endl;

}
