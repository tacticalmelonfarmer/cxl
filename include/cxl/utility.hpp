#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace cxl {

using index_t = long long signed;

inline namespace detail {

template<index_t, typename...>
struct ct_select;

template<typename T, typename... Ts>
struct ct_select<0, T, Ts...>
{
  using type = T;
};

template<index_t Index, typename T, typename... Ts>
struct ct_select<Index, T, Ts...>
{
  static_assert(Index <= sizeof...(Ts), "parameter pack index out of bounds");
  using type = typename ct_select<Index - 1, Ts...>::type;
};
}

template<index_t... Indices>
struct index_range
{
  constexpr auto size() const { return std::integral_constant<index_t, sizeof...(Indices)>{}; }
};

template<index_t Begin, index_t End, index_t Position = Begin, index_t... Result>
constexpr auto
make_index_range()
{
  if constexpr (Begin < End) {
    if constexpr (Position != End)
      return make_index_range<Begin, End, Position + 1, Result..., Position>();
    else
      return index_range<Result..., Position>{};
  } else if constexpr (Begin > End) {
    if constexpr (Position != End)
      return make_index_range<Begin, End, Position - 1, Result..., Position>();
    else
      return index_range<Result..., Position>{};
  } else
    return index_range<Position>{};
}

/*template<index_t First, index_t Last, index_t... Result>
constexpr auto
make_index_range()
{
  if constexpr (First == Last)
    return index_range<First>{};
  if constexpr (sizeof...(Result) == 0)
    make_index_range<First, Last, First>();
  else {
    constexpr index_t previous = (Result, ...);
    if constexpr (previous == Last)
      return index_range<Result...>{};
    else if constexpr (First < Last)
      return make_index_range<First, Last, Result..., previous + 1>();
    else if constexpr (First > Last)
      return make_index_range<First, Last, Result..., previous - 1>();
  }
}*/

template<typename Find, typename T0, typename... Ts, index_t Index>
constexpr auto
index_of(const std::integral_constant<index_t, Index> = std::integral_constant<index_t, 0>{})
{
  if constexpr (std::is_same_v<Find, T0>)
    return Index;
  else
    return index_of<Find, Ts...>(std::integral_constant<index_t, Index + 1>{});
}

template<index_t AtIndex, typename... Types>
using select_t = typename ct_select<AtIndex, Types...>::type;

template<auto N, auto E, index_t... Indices>
constexpr auto
pow_impl(const index_range<Indices...>) -> double
{
  constexpr float abs_pow = (1.0 * ... * (Indices, N));
  if constexpr (E > 0)
    return abs_pow;
  if constexpr (E < 0)
    return 1.0 / abs_pow;
  else
    return 1.0;
}

template<auto N, auto E>
constexpr auto
pow() -> double
{
  if constexpr (N == 0)
    return 0.0;
  if constexpr (E > 0)
    return pow_impl<N, E>(make_index_range<0, E - 1>());
  else
    return pow_impl<N, E>(make_index_range<0, E + 1>());
}

template<class... Ints>
constexpr index_t
combine_digits_base10(index_t result, index_t int0, Ints... ints)
{
  if constexpr (sizeof...(Ints) == 0)
    return result + int0;
  else
    return combine_digits_base10(result + (pow<10, sizeof...(Ints)>() * int0), ints...);
}

constexpr index_t
parse_digit(char C)
{
  return (C >= '0' && C <= '9') ? C - '0' : throw std::out_of_range("only decimal digits are allowed");
}
}
