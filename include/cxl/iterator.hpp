#pragma once
#include "utility.hpp"
#include <type_traits>

namespace cxl {

template<typename T, index_t Index>
struct iterator
{
  constexpr auto index() const { return std::integral_constant<index_t, Index>{}; }

  constexpr auto operator*() const { return T{}[std::integral_constant<size_t, Index>{}]; }
  constexpr auto operator++() const
  {
    static_assert(decltype(*this){} != T{}.end(), "iterator: index out of bounds");
    return iterator<T, Index + 1>{};
  }
  constexpr auto operator--() const
  {
    static_assert(decltype(*this){} != T{}.begin(), "iterator: index out of bounds");
    return iterator<T, Index - 1>{};
  }
  template<index_t D>
  constexpr bool operator==(const iterator<T, D>) const
  {
    return D == Index;
  }
  template<typename U>
  constexpr bool operator!=(const U) const
  {
    return !(decltype(*this){} == U{});
  }
};

template<typename T, index_t From, typename U, U Off>
constexpr auto
operator+(const iterator<T, From>, const std::integral_constant<U, Off>)
{
  // static_assert((From + Off) < T{}.end().index(), "iterator: index out of bounds");
  if constexpr ((From + Off) < T{}.end().index())
    return iterator<T, From + Off>{};
  else
    return T{}.end();
}

template<typename T, index_t From, typename U, U Off>
constexpr auto
operator-(const iterator<T, From>, const std::integral_constant<U, Off>)
{
  static_assert((From - Off) >= T{}.begin().index(), "iterator: index out of bounds");
  return iterator<T, From - Off>{};
}

template<typename T, index_t BeginIndex, index_t EndIndex>
constexpr auto
distance(const iterator<T, BeginIndex>, const iterator<T, EndIndex>)
{
  constexpr index_t raw = BeginIndex - EndIndex;
  return std::integral_constant < index_t, (raw < 0) ? (raw * -1) : raw > {};
}
}