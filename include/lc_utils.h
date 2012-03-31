/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#ifndef _LC_UTILS_H_
#define _LC_UTILS_H_

#include "lc_common.h"

#include <functional>
#include <tuple>
#include <vector>

namespace lc {

///////////////////////////////////////////////////////////////////////////
// seq/gens provide a variadic parameter pack of indices, useful for operations on tuples
//    Credit to Johannes Schaub for the solution @ stackoverflow
//http://stackoverflow.com/questions/7858817/unpacking-a-tuple-to-call-a-matching-function-pointer/7858971#7858971

template<int ...>
struct seq { };

template<int N, int ...S>
struct gens : gens<N-1, N-1, S...> { };

template<int ...S>
struct gens<0, S...> {
  typedef seq<S...> type;
}; 

///////////////////////////////////////////////////////////////////////////

struct deref {
  template<typename It>
  inline auto operator()(It& it) -> decltype( *it ) {
    return *it;
  }

  template<typename It>
  inline auto operator()(const It& it) -> decltype( *it ) {
    return *it;
  }
};

///////////////////////////////////////////////////////////////////////////

struct begin {
  template<typename C>
  inline auto operator()(C& c) -> decltype( std::begin(c) ) {
    return std::begin(c);
  }

  template<typename C>
  inline auto operator()(const C& c) -> decltype( std::begin(c) ) {
    return std::begin(c);
  }
};

///////////////////////////////////////////////////////////////////////////

struct end {
  template<typename C>
  inline auto operator()(C& c) -> decltype( std::end(c) ) {
    return std::end(c);
  }

  template<typename C>
  inline auto operator()(const C& c) -> decltype( std::end(c) ) {
    return std::end(c);
  }
};

///////////////////////////////////////////////////////////////////////////
// apply an operation to each element in a variadic tuple

template<int... S, typename TupleType, typename F>
inline auto apply(seq<S...>, TupleType&& t, F f) -> decltype( f(std::get<S>(t)...) ) {
  return f( std::get<S>(t)... );
}

template<int... S, typename TupleType, typename F>
inline auto apply(seq<S...>, const TupleType&& t, F& f) -> decltype( f(std::get<S>(t)...) ) {
  return f( std::get<S>(t)... );
}

///////////////////////////////////////////////////////////////////////////
// make a tuple out of the result of applying f to each element of the provided tuple

template<int ...S, typename TupleType, typename F> 
inline auto tuple_apply(seq<S...>, TupleType&& t, F f) -> decltype( std::make_tuple( f( std::get<S>(t) )... ) ) {
  return std::make_tuple( f( std::get<S>(t) )... );
}

template<int ...S, typename TupleType, typename F> 
inline auto tuple_apply(seq<S...>, const TupleType& t, F& f) -> decltype( std::make_tuple( f( std::get<S>(t) )... ) ) {
  return std::make_tuple( f( std::get<S>(t) )... );
}

///////////////////////////////////////////////////////////////////////////
// Given a tuple of iterators, provide a means for advancing over the cartesian product sequence
//    i.e., the last tuple iterator is the "inner-most" iterator, the first is the outer-most

template<size_t N> struct tuple_iterate;

template<> struct tuple_iterate<0> {
  template< typename... IterTypes >
  static bool next(const std::tuple<IterTypes...>& it, 
                   const std::tuple<IterTypes...>& beginIt, 
                   const std::tuple<IterTypes...>& endIt) {
    return false;
  }
};

template<size_t N> struct tuple_iterate {
  template< typename... IterTypes >
  static bool next(std::tuple<IterTypes...>& it, 
                   const std::tuple<IterTypes...>& beginIt, 
                   const std::tuple<IterTypes...>& endIt) {
    static const size_t i = sizeof...(IterTypes) - N;

    if(!tuple_iterate<N-1>::next(it, beginIt, endIt)) {
      if(++std::get<i>(it) == std::get<i>(endIt)) {
        std::get<i>(it) = std::get<i>(beginIt);
        return false;
      }
    }

    return true;
  }
};

template<typename... IterTypes>
bool iterate(std::tuple<IterTypes...>& it, 
             const std::tuple<IterTypes...>& beginIt, 
             const std::tuple<IterTypes...>& endIt) {
  // Next returns false if it has finished a complete cycle
  //      In that case, we manually reset the iterator to the end
  if(tuple_iterate<sizeof...(IterTypes)>::next(it, beginIt, endIt)) {
    return true;
  } else {
    it = endIt;
    return false;
  }
}

///////////////////////////////////////////////////////////////////////////
// Vector types

template<typename T>
struct types {
  typedef std::vector<T> list;
};

template< typename C >
auto vec(const C& c) -> typename types<typename std::remove_const<typename std::remove_reference<decltype(*std::begin(c))>::type>::type>::list {
  typedef typename std::remove_const<typename std::remove_reference<decltype(*std::begin(c))>::type>::type T;
  return typename types<T>::list(std::begin(c), std::end(c));
}

///////////////////////////////////////////////////////////////////////////

} // namespace lc

#endif // _LC_UTILS_H_