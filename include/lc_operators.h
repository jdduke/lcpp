/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#ifndef _LC_OPERATORS_H_
#define _LC_OPERATORS_H_

#include "lc_set.h"

#include <utility>

namespace lc {

///////////////////////////////////////////////////////////////////////////
// Setting the transform of the set
//
// e.g.: std::vector<int> a(10, 0), b(5, 1);
//       auto set = (std::plus<int> | from( a, b ));

template<typename F, typename R, typename... Args>
auto operator|(F f, cartesian<R,Args...>&& c) -> cartesian< decltype(f(std::declval<Args>()...)), Args...> {
  return cartesian<decltype(f(std::declval<Args>()...)),Args...>(std::move(c)).select(f);
}

///////////////////////////////////////////////////////////////////////////
// Adding filter conditions to the set
//
// e.g.:  std::vector<int> a(10, 0), b(5, 1);
//        auto set = (from( a, b ), [](int x, int y) { return x == y; });

template<typename R, typename... Args, typename F>
cartesian<R,Args...>& operator,(cartesian<R,Args...>& c, F f) {
  return c.where(f);
}

template<typename R, typename... Args, typename F>
cartesian<R,Args...> operator,(cartesian<R,Args...>&& c, F f) {
  return c.where(f);
}

///////////////////////////////////////////////////////////////////////////

} // namespace lc

#endif // _LC_OPERATORS_H_
