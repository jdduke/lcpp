/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <sstream>

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

  ///////////////////////////////////////////////////////////////////////////
  // Iterators

  auto set = (from(a, b), [](int x, const std::string& y) { return x == (y[0] - 'a'); });

  cout << "(x == (y[0] - 'a')) using iterators: " << endl;
  for (const auto& s : set)
    cout << s;

  cout << endl << endl;

  ///////////////////////////////////////////////////////////////////////////
  // Multiple sources and a custom transform

  auto combine = [](int x, const std::string& y) -> std::string {
    std::stringstream ss;
    ss << x << y;
    return ss.str();
  };
  cout << "(x+y):" << endl
            << (combine | from(a, b))()
            << endl << endl;

  ///////////////////////////////////////////////////////////////////////////
  // Custom (non-tuple result) transform

  auto cond = [](int x, const std::string& y) { return x == 0 || y[0] == 'a'; };
  cout << "Untransformed: " <<            (from(a, b), cond) () << endl;
  cout << "Transformed:   " << (combine | (from(a, b), cond))() << endl;
}

