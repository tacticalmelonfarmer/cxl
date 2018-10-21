#pragma once
#include "utility.hpp"
#include <type_traits>

namespace utility {

template<typename T, index_t Index>
struct citerator
{
  // typedef typename T::value_type value_type;
  const index_t index = Index;

  constexpr auto operator*() const { return T{}[std::integral_constant<size_t, Index>{}]; }
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
  template<index_t D>
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

template<typename T, index_t From, typename U, U Off>
constexpr auto
operator+(const citerator<T, From>, const std::integral_constant<U, Off>)
{
  return citerator<T, From + Off>{};
}

template<typename T, index_t From, typename U, U Off>
constexpr auto
operator-(const citerator<T, From>, const std::integral_constant<U, Off>)
{
  return citerator<T, From - Off>{};
}

template<typename T, index_t From, typename U, U Off>
constexpr auto
operator+(const std::integral_constant<U, Off>, const citerator<T, From>)
{
  return citerator<T, From + Off>{};
}

template<typename T, index_t From, typename U, U Off>
constexpr auto
operator-(const std::integral_constant<U, Off>, const citerator<T, From>)
{
  return citerator<T, From - Off>{};
}
}