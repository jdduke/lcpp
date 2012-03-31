#ifndef _LC_MAP_H_
#define _LC_MAP_H_

#include "lc_common.h"
#include "lc_utils.h"

namespace lc {

///////////////////////////////////////////////////////////////////////////

template< typename R, typename... Args >
func_wrapper<R,Args...> select(std::function<R(Args...)> f) {
  return func_wrapper<R,Args...>(f);
}

template< typename F > 
auto select(F f) -> decltype( typename function_traits<F>::wrapped_func_type(f) ) {
  return typename function_traits<F>::wrapped_func_type(f);
}

///////////////////////////////////////////////////////////////////////////

} // namespace lc

#endif // _LC_MAP_H_