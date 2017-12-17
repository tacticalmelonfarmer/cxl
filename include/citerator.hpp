#pragma once
#include "cindex.hpp"
#include "utility.hpp"
#include <type_traits>

namespace utility::cexpr {

template<typename T, size_t Index>
struct citerator
{
  typedef typename T::value_type value_type;
  const size_t index = Index;

  constexpr auto operator*() const { return T{}.data[Index]; }
  constexpr auto operator++() const
  {
    static_assert(decltype(*this){} != T{}.end(), "citerator: index out of bounds");
    return citerator<T, Index + 1>{};
  }
  constexpr auto operator--() const
  {
    static_assert(decltype(*this){} != T{}.begin(), "citerator: index out of bounds");
    return citerator<T, Index - 1>{};
  }
  constexpr bool operator==(const citerator<T, Index>&) const { return true; }
  template<size_t D>
  constexpr bool operator==(const citerator<T, D>&) const
  {
    return false;
  }
  template<typename U>
  constexpr bool operator!=(const U& u) const
  {
    return !(*this == u);
  }
};

template<typename T, size_t From, size_t Off>
constexpr auto
operator+(const citerator<T, From>&, const cindex<Off>&)
{
  return citerator<T, From + Off>{};
}

template<typename T, size_t From, size_t Off>
constexpr auto
operator-(const citerator<T, From>&, const cindex<Off>&)
{
  return citerator<T, From - Off>{};
}

template<typename T, size_t From, size_t Off>
constexpr auto
operator+(const cindex<Off>&, const citerator<T, From>&)
{
  return citerator<T, From + Off>{};
}

template<typename T, size_t From, size_t Off>
constexpr auto
operator-(const cindex<Off>&, const citerator<T, From>&)
{
  return citerator<T, From - Off>{};
}
}