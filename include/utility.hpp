#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace utility {

namespace detail {

template<bool E>
struct ct_require;

template<>
struct ct_require<true>
{};

template<>
struct ct_require<false>;

template<bool E, typename T, typename F>
struct ct_if_else;

template<typename T, typename F>
struct ct_if_else<false, T, F>
{
  typedef F type;
};

template<typename T, typename F>
struct ct_if_else<true, T, F>
{
  typedef T type;
};

template<size_t Index, typename... Ts>
struct ct_select;

template<typename T, typename... Ts>
struct ct_select<0, T, Ts...>
{
  typedef T type;
};

template<size_t Index, typename T, typename... Ts>
struct ct_select<Index, T, Ts...>
{
  static_assert(Index < sizeof...(Ts) + 1, "parameter pack index out of bounds");
  typedef typename ct_select<Index - 1, Ts...>::type type;
};

template<size_t Index, size_t End, size_t ResultSize, typename ResultType, bool AtEnd, typename T, typename... Ts>
struct ct_biggest_type_assist;

template<size_t Index, size_t End, size_t ResultSize, typename ResultType, typename T, typename... Ts>
struct ct_biggest_type_assist<Index, End, ResultSize, ResultType, true, T, Ts...> // end point
{
  static constexpr size_t this_size = sizeof(T);
  static constexpr size_t size = (this_size > ResultSize) ? this_size : ResultSize;
  typedef typename ct_if_else<(this_size > ResultSize), T, ResultType>::type type;
};

template<size_t Index, size_t End, size_t ResultSize, typename ResultType, typename T, typename... Ts>
struct ct_biggest_type_assist<Index, End, ResultSize, ResultType, false, T, Ts...>
{
  typedef ct_biggest_type_assist<Index + 1, End, ResultSize, ResultType, (Index + 1 == End), Ts...> next;
  static constexpr size_t this_size = sizeof(T);
  static constexpr size_t size =
    (this_size > next::this_size)
      ?
      /*then*/ ct_biggest_type_assist<Index + 1, End, this_size, T, (Index + 1 == End), Ts...>::size
      :
      /*else*/ next::size;
  typedef
    typename ct_if_else<(this_size > next::this_size),
                        /*then*/
                        typename ct_biggest_type_assist<Index + 1, End, this_size, T, (Index + 1 == End), Ts...>::type,
                        /*else*/ typename next::type>::type type;
};

template<typename... Ts>
struct ct_biggest_type; // entry point -> ct_biggest_type_assist

template<>
struct ct_biggest_type<>
{
  constexpr static size_t size = 0;
  typedef void type;
};

template<typename T>
struct ct_biggest_type<T>
{
  constexpr static size_t size = sizeof(T);
  typedef T type;
};

template<typename T, typename... Ts>
struct ct_biggest_type<T, Ts...>
{
  constexpr static size_t size =
    ct_biggest_type_assist<0, sizeof...(Ts), 0, void, (sizeof...(Ts) == 0), T, Ts...>::size;
  typedef typename ct_biggest_type_assist<0, sizeof...(Ts), 0, void, (sizeof...(Ts) == 0), T, Ts...>::type type;
};

template<auto A, auto B>
struct cpow
{
  static const int value = A * cpow<A, B - 1>::value;
};
template<auto A>
struct cpow<A, 0>
{
  static const int value = 1;
};

template<class... Ints>
constexpr size_t
combine_digits_base10(size_t result, size_t int0, Ints... ints)
{
  if constexpr (sizeof...(Ints) == 0)
    return result + int0;
  else
    return combine_digits_base10(result + (cpow<10, sizeof...(Ints)>::value * int0), ints...);
}

constexpr size_t
parse_digit(char C)
{
  return (C >= '0' && C <= '9') ? C - '0' : throw std::out_of_range("only decimal digits are allowed");
}
}

using namespace detail;

struct Auto
{};

template<size_t... Indices>
struct index_range
{
  const std::integral_constant<size_t, sizeof...(Indices)> size = {};
};

template<size_t Begin, size_t End, size_t Position = Begin, size_t... Result>
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

template<typename Find, typename T0, typename... Ts, size_t Index>
constexpr auto
index_of(const std::integral_constant<size_t, Index> = std::integral_constant<size_t, 0>{})
{
  if constexpr (std::is_same_v<Find, T0>)
    return Index;
  else
    return index_of<Find, Ts...>(std::integral_constant<size_t, Index + 1>{});
}

template<size_t AtIndex, typename... TypeList>
using select_t = typename ct_select<AtIndex, TypeList...>::type;

template<bool Expression, typename Then, typename Else>
using if_else_t = typename ct_if_else<Expression, Then, Else>::type;

template<typename F, template<class...> typename T, size_t... Indices, class... TL>
constexpr decltype(auto)
apply_impl(F&& functional, T<TL...>&& tuple, index_range<Indices...>&& indices)
{
  return functional(std::get<Indices>(tuple)...);
}

template<typename F, template<class...> typename T, class T0, class... TL>
constexpr decltype(auto)
apply(F&& functional, T<T0, TL...>& tuple)
{
  return apply_impl(
    std::forward<F>(functional), std::forward<T<T0, TL...>>(tuple), make_index_range<0, sizeof...(TL)>());
}

template<typename F, template<class...> typename T>
constexpr decltype(auto)
apply(F&& functional, T<>& tuple)
{
  return std::forward<F>(functional)();
}
}
