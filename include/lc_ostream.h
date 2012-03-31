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

template<std::size_t> struct size_ { };

template<class Ch, class Tr, class T>
std::basic_ostream<Ch,Tr>& operator<<(std::basic_ostream<Ch,Tr>& os,
                                      const std::vector<T>& v) {
  os << "(";
  for (auto it = std::begin(v); it != std::end(v); ++it) {
    os << *it;
    if ((it+1) != std::end(v))
      os << ", ";
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

template<class Ch, class Traits, class... Args>
std::ostream& operator<<(std::basic_ostream<Ch,Traits>& os,
                         std::tuple<Args...> const& t) {
  os << "(";
  print_tuple(os, t, size_<sizeof...(Args)-1>());
  return os << ")";
}

///////////////////////////////////////////////////////////////////////////

} // namespace lc

#endif // _LC_OSTREAM_H_