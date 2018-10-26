#pragma once
#include "typelist.hpp"
#include "utility.hpp"

namespace cxl {

inline namespace detail {

template<index_t Index, typename Type>
struct tuple_element
{
  const Type value;

  constexpr tuple_element(const Type Init)
    : value(Init)
  {}

  constexpr auto get() const { return value; }
};

template<typename IRange, typename... Types>
struct tuple_impl;

template<typename... Types, index_t... Indices>
struct tuple_impl<index_range<Indices...>, Types...> : private ctuple_element<Indices, Types>...
{
  const typelist<Types...> types;

  constexpr auto size() const { return std::integral_constant<index_t, sizeof...(Types)>{}; }

  constexpr tuple_impl(const Types... Init)
    : tuple_element<Indices, Types>(Init)...
  {}

  template<index_t Index>
  constexpr auto get(const std::integral_constant<index_t, Index>)
  {
    return tuple_element<Index, decltype(types.type_at(std::integral_constant<index_t, Index>{}))>::get();
  }

  template<index_t Index>
  constexpr auto operator[](const std::integral_constant<index_t, Index>)
  {
    return get(std::integral_constant<index_t, Index>{});
  }
};
}

template<typename... Types>
using tuple = tuple_impl<decltype(make_index_range<0, sizeof...(Types) - 1>()), Types...>;
}