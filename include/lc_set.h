/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012, Jared Duke.
// This code is released under the MIT License.
// www.opensource.org/licenses/mit-license.php
/////////////////////////////////////////////////////////////////////////////

#ifndef _LC_SET_H_
#define _LC_SET_H_

#include "lc_common.h"
#include "lc_utils.h"

#include <iterator>

namespace lc {

template< typename R, typename... Args >
struct cartesian {

  typedef std::tuple<typename types<Args>::list...>            tuple_list;
  typedef std::tuple<typename types<Args>::list::iterator...>       tuple_iter;
  typedef std::tuple<typename types<Args>::list::const_iterator...> const_tuple_iter;
  typedef typename gens<sizeof...(Args)>::type                 tuple_indices;
  typedef decltype(tuple_apply(tuple_indices(), tuple_iter(),  deref())) tuple_value;

  typedef std::function<bool(Args...)>      filter_type;
  typedef typename types<filter_type>::list filter_list;

  typedef std::function<R(Args...)> transform_type;
  typedef typename types<R>::list   result_list;

  template< typename > class iterator;
  template< typename > friend class iterator;
  template< typename,typename... > friend class cartesian;

  ///////////////////////////////////////////////////////////////////////////


  cartesian(const typename types<Args>::list&... args)
    : mLists(std::make_tuple(args...)),
      mTransform(std::make_tuple<Args...>) { }

  cartesian(const typename types<Args>::list&... args, std::initializer_list<filter_type> l)
    : mLists(std::make_tuple(args...)),
    mFilters(l),
    mTransform(std::make_tuple<Args...>) { }

  cartesian(typename types<Args>::list&&... args)
    : mLists(std::make_tuple(std::move(args)...)),
    mTransform(std::make_tuple<Args...>) { }

  cartesian(typename types<Args>::list&&... args, std::initializer_list<filter_type> l)
    : mLists(std::make_tuple(std::move(args)...)),
    mFilters(l),
    mTransform(std::make_tuple<Args...>) { }

  cartesian(cartesian&& other) {
    swap(std::move(other));
  }

  cartesian& operator=(cartesian&& other) {
    swap(std::move(other));
  }

  cartesian(const cartesian& other) 
    : mLists(other.mLists),
      mFilters(other.mFilters),
      mTransform(other.mTransform) {

  }


  template<typename R2>
  cartesian(const cartesian<R2,Args...>& other) 
    : mLists(other.mLists),
    mFilters(other.mFilters) {

  }

  template<typename R2>
  cartesian(cartesian<R2,Args...>&& other) {
    std::swap(mLists,   other.mLists);
    std::swap(mFilters, other.mFilters);
  }

  ///////////////////////////////////////////////////////////////////////////

  template< typename F >
  cartesian& where(F f) {
    mFilters.push_back(filter_type(f));
    return *this;
  }

  template< typename F >
  cartesian& where(std::initializer_list<F> fs) {
    std::copy(std::begin(fs), std::end(fs), std::back_inserter(mFilters));
    return *this;
  }

  template< typename F >
  cartesian& select(F f) {
    mTransform = transform_type(f);
    return *this;
  }

  result_list operator()() {
    result_list results;
    auto back = back_inserter(results);

    for (auto b = tupleBegin(), it = tupleBegin(), e = tupleEnd(); iterate(it, b, e); )
      if( filter(it) ) 
        back = transform(it);

    return results;
  }

  iterator<const_tuple_iter> begin() const { return iterator<const_tuple_iter>(tupleBegin(), *this); }
  iterator<const_tuple_iter> end()   const { return iterator<const_tuple_iter>(tupleEnd(),   *this); }

  iterator<tuple_iter> begin() { return iterator<tuple_iter>(tupleBegin(), *this); }
  iterator<tuple_iter> end()   { return iterator<tuple_iter>(tupleEnd(),   *this); }

private:

  cartesian();
  cartesian& operator=(const cartesian&);

  void swap(cartesian&& other) {
    std::swap(mLists,     other.mLists);
    std::swap(mFilters,   other.mFilters);
    std::swap(mTransform, other.mTransform);
  }

  static tuple_value valueOf(tuple_iter it) {
    return tuple_apply(tuple_indices(), it,     deref());
  }

  static tuple_value valueOf(const_tuple_iter it) {
    return tuple_apply(tuple_indices(), it,     deref());
  }

  tuple_iter tupleBegin() {
    return tuple_apply(tuple_indices(), mLists, ::lc::begin());
  }

  const_tuple_iter tupleBegin() const {
    return tuple_apply(tuple_indices(), mLists, ::lc::begin());
  }

  tuple_iter tupleEnd() {
    return tuple_apply(tuple_indices(), mLists, ::lc::end());
  }

  const_tuple_iter tupleEnd() const {
    return tuple_apply(tuple_indices(), mLists, ::lc::end());
  }

  bool filter(tuple_iter& it) const {
    const auto v = valueOf(it);
    for (auto f = std::begin(mFilters); f != std::end(mFilters); ++f) {
      if (!apply(tuple_indices(), v, *f))
        return false;
    }
    return true;
  }

  bool filter(const_tuple_iter& it) const {
    const auto v = valueOf(it);
    for (auto f = std::begin(mFilters); f != std::end(mFilters); ++f) {
      if (!apply(tuple_indices(), v, *f))
        return false;
    }
    return true;
  }

  R transform(tuple_iter it) const {
    return apply(tuple_indices(), valueOf(it), mTransform);
  }

  R transform(const_tuple_iter it) const {
    return apply(tuple_indices(), valueOf(it), mTransform);
  }

  ///////////////////////////////////////////////////////////////////////////

  tuple_list  mLists;
  filter_list mFilters;
  transform_type  mTransform;

};

template< typename R, typename... Args>
template< typename It >
class cartesian<R,Args...>::iterator : public std::iterator<std::forward_iterator_tag, R, ptrdiff_t, const R*, const R&> {
public:

  iterator(It it, cartesian& c)
    : mIt(it), mBegin(c.tupleBegin()), mEnd(c.tupleEnd()), mDirty(true), mSource(c) { advanceBegin(); }

  iterator(It it, const cartesian& c)
    : mIt(it), mBegin(c.tupleBegin()), mEnd(c.tupleEnd()), mDirty(true), mSource(c) { advanceBegin(); }

  iterator(const iterator& other) 
    : mIt(other.mIt), mBegin(other.mBegin), mEnd(other.mEnd), mDirty(other.mDirty), mSource(other.mSource) { }

  const R& operator*() const  { update();  return  mValue; }
  const R* operator->() const { update();  return &mValue; }
  iterator& operator++()           { advance(); return *this;   }

  bool operator==(const iterator& other) { return mIt == other.mIt; }
  bool operator!=(const iterator& other) { return mIt != other.mIt; }
  operator bool() const { return mIt != mEnd; }

private:

  void update() const {
    if (!mDirty)
      return;

    mValue = mSource.transform(mIt);
    mDirty = false;
  }

  void advance(size_t count = 1) {
    mDirty = count > 0;
    while (count > 0 && iterate(mIt, mBegin, mEnd)) {
      if(mSource.filter(mIt))
        --count;
    }
  }

  void advanceBegin() {
    if(!mSource.filter(mIt))
      advance();
  }

  It  mIt;
  It  mBegin;
  It  mEnd;
  mutable R   mValue;
  mutable bool     mDirty;
  const cartesian& mSource;
};

///////////////////////////////////////////////////////////////////////////

template<typename... Args> 
cartesian<std::tuple<Args...>, Args...> from(const typename types<Args>::list&... args) {
  return cartesian<std::tuple<Args...>, Args...>(args...);
}

template<typename... Args> 
cartesian<std::tuple<Args...>, Args...> from(typename types<Args>::list&&... args) {
  return cartesian<std::tuple<Args...>, Args...>(std::move(args)...);
}

template< size_t... Sizes, typename... Args> 
cartesian<std::tuple<Args...>, Args...> from(std::array<Args,Sizes>... args) {
  return cartesian<std::tuple<Args...>, Args...>(vec(args)...);
}

template< template<typename T> class C, typename... Args> 
cartesian<std::tuple<Args...>, Args...> from(C<Args>... args) {
  return cartesian<std::tuple<Args...>, Args...>(vec(args)...);
}

///////////////////////////////////////////////////////////////////////////

} // namespace lc

#endif // _LC_SET_H_