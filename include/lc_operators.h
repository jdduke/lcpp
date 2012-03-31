#ifndef _LC_OPERATORS_H_
#define _LC_OPERATORS_H_

#include "lc_map.h"
#include "lc_set.h"
#include "lc_filter.h"

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

template<typename R, typename R2, typename... Args>
cartesian<R,Args...>& operator|(func_wrapper<R,Args...> s, const cartesian<R2,Args...>& c) {
  return cartesian<R,Args...>(c).select(s);
}

template<typename R, typename R2, typename... Args>
cartesian<R,Args...> operator|(func_wrapper<R,Args...> s, cartesian<R2,Args...>&& c) {
  return cartesian<R,Args...>(c).select(s.mF);
}

///////////////////////////////////////////////////////////////////////////

} // namespace lc

#endif // _LC_OPERATORS_H_