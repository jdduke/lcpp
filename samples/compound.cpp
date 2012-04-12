/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <array>
#include <iostream>
#include <string>

#include "lcpp.h"

int main(int argc, char* argv[]) {

  using namespace lc;
  using namespace std;

  ///////////////////////////////////////////////////////////////////////////
  // arrays, vectors and other templated containers are all supported

  const array<int,    10> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  const array<string, 5>  b = {"a", "b", "c", "d", "e"};


  ///////////////////////////////////////////////////////////////////////////
  // Multiple sources, single condition

  cout << "x < 5, y != 'c': " << endl
            << (from(a, b) , [](int x, const std::string& y) { return x < 5 && y[0] != 'c'; })()
            << endl << endl;


  ///////////////////////////////////////////////////////////////////////////
  // Multiple sources, multiple conditions

  cout << "(x > 5) && (y == z): " << endl
            << (from(a, b, b) , [](int x, const std::string&,   const std::string&)   { return x > 5; }
                              , [](int,   const std::string& y, const std::string& z) { return y == z; })()
            << endl<< endl;

  // TODO
#if 0

  ///////////////////////////////////////////////////////////////////////////
  // Iterators

  auto list = (from(a, b));//, [](int x, const std::string& y) { return true; x == (y[0] - 'a'); });

  cout << "(x == (y-'a')) using iterators: " << endl;
  for(auto it = begin(list); it != end(list); ++it) {
    cout << *it;
  }

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
#endif

}

