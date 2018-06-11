#pragma once
#include "typelist.hpp"
#include "utility.hpp"

namespace utility {

namespace detail {

template<size_t Index, typename Type>
struct ctuple_element
{
  const Type value;

  constexpr ctuple_element(const Type Init)
    : value(Init)
  {}

  constexpr auto get() const { return value; }
};

template<typename IRange, typename... Types>
struct ctuple;

template<typename... Types, size_t... Indices>
struct ctuple<index_range<Indices...>, Types...> : private ctuple_element<Indices, Types>...
{
  const typelist<Types...> types;
  const std::integral_constant<size_t, sizeof...(Types)> size;

  constexpr ctuple(const Types... Init)
    : ctuple_element<Indices, Types>(Init)...
  {}

  template<size_t Index>
  constexpr auto get(const std::integral_constant<size_t, Index>)
  {
    return ctuple_element<Index, decltype(types.type_at(std::integral_constant<size_t, Index>{}))>::get();
  }

  template<size_t Index>
  constexpr auto operator[](const std::integral_constant<size_t, Index>)
  {
    return get(std::integral_constant<size_t, Index>{});
  }
};
}

template<typename... Types>
using ctuple = detail::ctuple<decltype(make_index_range<0, sizeof...(Types) - 1>()), Types...>;
}