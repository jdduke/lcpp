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


// A utility container for generating list comprehensions
//    Stores the following:
//      1) The source lists
//      2) A set of filters for tuples generated by the product set of the source lists
//      3) A transform* that maps candidate tuples to output values
//           The default transform simply yields the filtered tuple
template< typename R, typename... Args >
struct product_set {

  typedef std::tuple<typename types<Args>::list...>                 tuple_list;
  typedef std::tuple<typename types<Args>::list::iterator...>       tuple_iter;
  typedef std::tuple<typename types<Args>::list::const_iterator...> const_tuple_iter;
  typedef typename gens<sizeof...(Args)>::type                      tuple_indices;
  typedef decltype(tuple_apply(tuple_indices(), tuple_iter(),  deref())) tuple_value;

  typedef std::function<bool(Args...)>      filter_type;
  typedef typename types<filter_type>::list filter_list;

  typedef std::function<R(Args...)> transform_type;
  typedef typename types<R>::list   result_list;

  template< typename > class iterator;
  template< typename > friend class iterator;
  template< typename,typename... > friend class product_set;

  ///////////////////////////////////////////////////////////////////////////
  // Constructors

  // Construct from lists of types Args...
  product_set(typename types<Args>::list... args)
    : mLists(std::make_tuple(std::move(args)...)),
      mTransform(std::make_tuple<Args...>) { }

  // Construct from lists of types Args..., and a list of filters
  product_set(typename types<Args>::list... args, std::initializer_list<filter_type> l)
    : mLists(std::make_tuple(std::move(args)...)),
      mFilters(std::move(l)),
      mTransform(std::make_tuple<Args...>) { }

  // Copy move constructor
  product_set(product_set&& other) {
    swap(std::move(other));
  }

  // Copy move constructor from set with different transform type
  template<typename R2>
  product_set(product_set<R2,Args...>&& other) {
    std::swap(mLists,   other.mLists);
    std::swap(mFilters, other.mFilters);
  }

  // Assignment operator
  product_set& operator=(product_set&& other) {
    swap(std::move(other));
  }


  ///////////////////////////////////////////////////////////////////////////
  // Methods

  // Adds a filter to the set, no processing is performed
  //    The filter must have a signature of type:   bool(Args...)
  template< typename F >
  product_set& where(F f) {
    mFilters.push_back(filter_type(f));
    return *this;
  }

  // Adds a list of filters to the set, no processing is performed
  //    Each filter must have a signature of type:  bool(Args...)
  template< typename F >
  product_set& where(std::initializer_list<F> fs) {
    std::copy(std::begin(fs), std::end(fs), std::back_inserter(mFilters));
    return *this;
  }

  // Sets the transform for the set, no processing is performed
  //    The transform must have a signature of type: R(Args...)
  template< typename F >
  product_set& select(F f) {
    mTransform = transform_type(f);
    return *this;
  }

  // Return the set formed by the product set of the associated lists, where
  //    1) Each candidate tuple from the source lists satisfies all filter conditions and
  //    2) Each set element is the result of transforming the candidate tuple by the member transform
  result_list operator()() const {
    result_list results;

#if 1
    for (const auto& r : *this)
      results.push_back(r);
#else
    auto back = back_inserter(results);
    for (auto b = tupleBegin(), it = tupleBegin(), e = tupleEnd(); it != e; iterate(it, b, e) )
       if (filter(it))
        back = transform(it);
#endif

    return std::move(results);
  }

  // Return an iterator to the first element of the output set
  //   Note: Set generation is lazy.  Filtering and transforming is only performed
  //         as the iterator is incremented and dereferenced.
  iterator<const_tuple_iter> begin() const { return iterator<const_tuple_iter>(tupleBegin(), *this, true); }

  // Return an iterator to the end of the output set
  //   Note: This does not refer to a valid value, and should not be dereferenced
  iterator<const_tuple_iter> end()   const { return iterator<const_tuple_iter>(tupleEnd(),   *this); }

private:

  product_set();
  product_set(const product_set&);
  product_set& operator=(const product_set&);

  void swap(product_set&& other) {
    std::swap(mLists,     other.mLists);
    std::swap(mFilters,   other.mFilters);
    std::swap(mTransform, other.mTransform);
  }

  // Create a tuple of values from a tuple of const iterators
  static tuple_value valueOf(const_tuple_iter it) {
    return tuple_apply(tuple_indices(), it,     deref());
  }

  // Create a tuple of const iterators to the beginnings of each list in a tuple of lists
  const_tuple_iter tupleBegin() const {
    return tuple_apply(tuple_indices(), mLists, container_begin());
  }

  // Create a tuple of const iterators to the ends of each list in a tuple of lists
  const_tuple_iter tupleEnd() const {
    return tuple_apply(tuple_indices(), mLists, container_end());
  }

  // Return whether the value referenced by the const iterator satisifes all filter conditions
  bool filter(const_tuple_iter& it) const {
    const auto v = valueOf(it);
    for (auto f = std::begin(mFilters); f != std::end(mFilters); ++f) {
      if (!apply(tuple_indices(), v, *f))
        return false;
    }
    return true;
  }

  // Transform the value referenced by the const iterator according to the member transform
  R transform(const_tuple_iter it) const {
    return apply(tuple_indices(), valueOf(it), mTransform);
  }

  ///////////////////////////////////////////////////////////////////////////

  tuple_list     mLists;     // A tuple containing the user-provided lists of data
  filter_list    mFilters;   // A list of filters for the candidate tuples
  transform_type mTransform; // A transform that maps filtered tuples to the output set
};

// An iterator to the set that yields filtered and transformed set elements
//    List filtering is performed as the iterator is incremented
//    List transforming is performed only when the iterator is dereferenced
template< typename R, typename... Args>
template< typename It >
class product_set<R,Args...>::iterator : public std::iterator<std::forward_iterator_tag, R, ptrdiff_t, const R*, const R&> {
public:

  iterator() : mDirty(false), mSource(nullptr) { }

  iterator(It it, const product_set& c, bool advanceToBegin = false)
    : mIt(it),
      mBegin(c.tupleBegin()),
      mEnd(c.tupleEnd()),
      mDirty(true),
      mSource(&c) { if (advanceToBegin) advanceBegin(); }

  iterator(const iterator& other)
    : mIt(other.mIt),
      mBegin(other.mBegin),
      mEnd(other.mEnd),
      mValue(other.mValue),
      mDirty(other.mDirty),
      mSource(other.mSource) { }

  iterator(iterator&& other) {
    swap(std::move(other));
  }

  iterator& operator=(iterator other) {
    swap(std::move(other));
  }

  const R&  operator*()  const { update();  return  mValue; }
  const R*  operator->() const { update();  return &mValue; }
  iterator& operator++()       { advance(); return *this;   }

  bool operator==(const iterator& other) const { return mIt == other.mIt; }
  bool operator!=(const iterator& other) const { return mIt != other.mIt; }
       operator bool()                   const { return mIt != mEnd; }

private:

  bool valid() const { return nullptr != mSource; }

  void swap(iterator&& other) {
    using std::swap;
    swap(mIt,     other.mIt);
    swap(mBegin,  other.mBegin);
    swap(mEnd,    other.mEnd);
    swap(mValue,  other.mValue);
    swap(mSource, other.mSource);
    swap(mDirty,  other.mDirty);
  }

  void update() const {
    if (!valid() || !mDirty)
      return;

    mValue = mSource->transform(mIt);
    mDirty = false;
  }

  void advance(size_t count = 1) {
    if (!valid())
      return;

    while (count > 0 && iterate(mIt, mBegin, mEnd)) {
      mDirty = true;
      if(mSource->filter(mIt))
        --count;
    }
  }

  void advanceBegin() {
    if(mIt != mEnd && !mSource->filter(mIt))
      advance();
  }

  It  mIt;
  It  mBegin;
  It  mEnd;
  mutable R          mValue;
  mutable bool       mDirty;
  const product_set* mSource;
};

///////////////////////////////////////////////////////////////////////////
// Set creation
//
// from(x,y,z) creates a product set from the lists x,y and z

// Create a product set from the specified movable vectors, not necessarily of the same underlying type
template<typename... Args>
product_set<std::tuple<Args...>, Args...> from(std::vector<Args>... args) {
  return product_set<std::tuple<Args...>, Args...>(std::move(args)...);
}

// Create a product set from the specified arrays, not necessarily of the same underlying type
template< size_t... Sizes, typename... Args>
product_set<std::tuple<Args...>, Args...> from(const std::array<Args,Sizes>&... args) {
  return product_set<std::tuple<Args...>, Args...>(toList(args)...);
}

// Create a product set from the specified containers, not necessarily of the same underlying type
//    Each container must have valid usage with std::begin() and std::end()
template< template<typename T> class C, typename... Args>
product_set<std::tuple<Args...>, Args...> from(C<Args>... args) {
  return product_set<std::tuple<Args...>, Args...>(toList(args)...);
}

///////////////////////////////////////////////////////////////////////////

} // namespace lc

#endif // _LC_SET_H_
