#pragma once

#include <tuple>
#include <type_traits>
#include <utility>

namespace cxl {

using std::size_t;

inline namespace detail {

template<typename T>
struct size_greater
{
  static constexpr auto size = sizeof(T);
};

template<typename T, typename U>
constexpr auto
operator>(const size_greater<T>, const size_greater<U>)
{
  (sizeof(T) >= sizeof(U)) ? size_greater<T>{} : size_greater<U>{};
}

template<typename... Types>
struct max_size
{
  static constexpr auto value = (size_greater<Types>{} > ...).size;
};

struct unknown_t
{
  template<typename T>
  operator T() const;
};

template<size_t N = 0>
using unknown = const unknown_t;

template<typename T, size_t... I>
constexpr auto
is_brace_constructible_(std::index_sequence<I...>, T*) -> decltype(T{ unknown<I>{}... }, std::true_type{})
{
  return {};
}

template<size_t... I>
constexpr std::false_type
is_brace_constructible_(std::index_sequence<I...>, ...)
{
  return {};
}

template<typename T, size_t N>
constexpr auto
is_brace_constructible() -> decltype(is_brace_constructible_(std::make_index_sequence<N>{}, static_cast<T*>(nullptr)))
{
  return {};
}

template<size_t N>
using size = std::integral_constant<size_t, N>;

template<typename A, typename... Ts>
constexpr auto
make_tuple(Ts&&... Values)
{
  if constexpr (std::is_rvalue_reference_v<A>)
    return std::make_tuple(std::forward<Ts>(Values)...);
  else if constexpr (std::is_lvalue_reference_v<A>)
    return std::forward_as_tuple(std::forward<Ts>(Values)...);
}
}

template<typename T, size_t Count = 0>
constexpr auto
aggregate_arity()
{
  if constexpr (std::is_aggregate_v<T> && is_brace_constructible<T, Count>() && !is_brace_constructible<T, Count + 1>())
    return size<Count>{};
  else
    return aggregate_arity<T, Count + 1>();
}

template<typename T>
constexpr auto
destructure(T&& pod)
{
  constexpr auto arity = aggregate_arity<std::remove_reference_t<T>>();
  if constexpr (arity == 0)
    return std::tuple<>{};

// this included file is generated by a custom build tool using cmake
#include "aggregate.generated.h"
}
}