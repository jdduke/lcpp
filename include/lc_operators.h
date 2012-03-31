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

template<typename R, typename... Args, typename F>
cartesian<R,Args...>& operator|(cartesian<R,Args...>& c, F f) {
  return c.where(f);
}

template<typename R, typename... Args, typename F>
cartesian<R,Args...>& operator,(cartesian<R,Args...>& c, F f) {
  return c.where(f);
}

template<typename R, typename... Args, typename F>
cartesian<R,Args...>& operator>>=(cartesian<R,Args...>& c, F f) {
  return c.select(f);
}

///////////////////////////////////////////////////////////////////////////

template<typename R, typename... Args, typename F>
cartesian<R,Args...> operator|(cartesian<R,Args...>&& c, F f) {
  return c.where(f);
}

template<typename R, typename... Args, typename F>
cartesian<R,Args...> operator,(cartesian<R,Args...>&& c, F f) {
  return c.where(f);
}

template<typename R, typename... Args, typename F>
cartesian<R,Args...> operator>>=(cartesian<R,Args...>&& c, F f) {
  return c.select(f);
}

///////////////////////////////////////////////////////////////////////////

template<typename F, typename R, typename... Args>
auto operator|(F f, cartesian<R,Args...>&& c) -> cartesian< decltype(f(std::declval<Args>()...)), Args...> {
  return cartesian<decltype(f(std::declval<Args>()...)),Args...>(c).select(f);
}

///////////////////////////////////////////////////////////////////////////

} // namespace lc

#endif // _LC_OPERATORS_H_