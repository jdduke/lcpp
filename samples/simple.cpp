#include <array>
#include <iostream>

#include "lcpp.h"

int main(int argc, char* argv[]) {

  using namespace lc;

  ///////////////////////////////////////////////////////////////////////////
  // arrays, vectors and other templated containers are all supported
  std::array<int, 10> a = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  std::array<int, 10> b = {9, 8, 7, 6, 5, 4, 3, 2, 1, 0};


  ///////////////////////////////////////////////////////////////////////////
  // Multiple sources, single condition
  std::cout << (from(a, b) , [](int x, int y) { return x < y; })()
            << std::endl << std::endl;


  ///////////////////////////////////////////////////////////////////////////
  // Multiple sources, multiple conditions

  std::cout << (from(a, a, a) , [](int x, int y, int z) { return x + y < z; }
                              , [](int x, int y, int z) { return x > y; })()
            << std::endl<< std::endl;


  ///////////////////////////////////////////////////////////////////////////
  // Multiple sources, multiple conditions and a selecting transform

  std::cout << ((from(a, a, a) , [](int x, int y, int z) { return x + y + z == 10; })
                             >>= [](int x, int y, int z) { return std::make_tuple(x+y+z, x+y+z, x+y+z); })()
            << std::endl<< std::endl;


  ///////////////////////////////////////////////////////////////////////////
  // Iterators

  auto list = (from(a, b), [](int x, int y) { return x == y; });

  for(auto it = list.begin(); it != list.end(); ++it) {
    std::cout << *it;
  }

  std::cout << std::endl << std::endl;


  ///////////////////////////////////////////////////////////////////////////
  // Custom (non-tuple result) transform

  auto tf   = [](int x, int y) { return x + y;  };
  auto cond = [](int x, int y) { return x == y; };
  std::cout << "Untransformed: " << (from(a, b), cond)() << std::endl;
  std::cout << "Transformed:   " << (select(tf) | (from(a, b), cond))() << std::endl;

}
