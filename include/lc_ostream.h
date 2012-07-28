/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#ifndef _LC_OSTREAM_H_
#define _LC_OSTREAM_H_

#include <iostream>
#include <tuple>
#include <vector>

namespace lc {

///////////////////////////////////////////////////////////////////////////
// Provide ostream-based output of vectors, and variadic tuples

template<std::size_t> struct size_ { };

template<class Ch, class Traits, class... Args>
std::ostream& operator<<(std::basic_ostream<Ch,Traits>& os,
                         std::tuple<Args...> const& t) {
  os << "(";
  print_tuple(os, t, size_<sizeof...(Args)-1>());
  return os << ")";
}

template<class Ch, class Tr, class T>
std::basic_ostream<Ch,Tr>& operator<<(std::basic_ostream<Ch,Tr>& os,
                                      const std::vector<T>& v) {
  os << "(";
  const auto begin = std::begin(v);
  const auto end   = std::end(v);
  for (auto it = begin; it != end; ++it) {
    if (it != begin)
      os << ", ";
    os << *it;
  }
  os << ")";
  return os;
}

template<class Ch, class Tr, class Tuple, std::size_t I>
void print_tuple(std::basic_ostream<Ch,Tr>& os, Tuple const& t, size_<I>) {
  print_tuple(os, t, size_<I-1>());
  os << ", " << std::get<I>(t);
}

template<class Ch, class Tr, class Tuple>
void print_tuple(std::basic_ostream<Ch,Tr>& os, Tuple const& t, size_<0>) {
  os << std::get<0>(t);
}

struct printer {
  typedef void value_type;

  template <typename T>
  printer& operator=(const T& t) {
    std::cout << t; return *this;
  }

  printer& operator++() {
    return *this;
  }
};

///////////////////////////////////////////////////////////////////////////

} // namespace lc

#endif // _LC_OSTREAM_H_
