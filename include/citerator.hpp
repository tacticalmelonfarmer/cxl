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
  constexpr auto operator++() const { return citerator<T, Index + 1>{}; }
  constexpr auto operator--() const { return citerator<T, Index - 1>{}; }
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