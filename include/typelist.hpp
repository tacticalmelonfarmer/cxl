#pragma once

#include "cintegral.hpp"
#include "utility.hpp"
#include <type_traits>

namespace utility {

template<typename... Ts>
struct typelist;

namespace detail {

template<typename... Ts>
struct tl_helper;

template<>
struct tl_helper<>
{
  using head_type = void;
  using next_types = void;
};

template<typename T0, typename... Ts>
struct tl_helper<T0, Ts...>
{
  using head_type = T0;
  using next_types = typelist<Ts...>;
};
}

using namespace detail;

template<>
struct typelist<>
{
  using head_type = void;
  using next_types = void;
  const size_t size = 0;
  const size_t sizeof_min = 0;
  const size_t sizeof_max = 0;
  const size_t alignof_min = 0;
  const size_t alignof_max = 0;

  template<template<typename...> typename TL, typename... Deduced>
  constexpr auto append(TL<Deduced...>) const
  {
    return typelist<Deduced...>{};
  }

  template<template<typename...> typename TL, typename... Deduced>
  constexpr auto prepend(TL<Deduced...>) const
  {
    return typelist<Deduced...>{};
  }

  template<template<typename...> typename ApplyTo>
  constexpr auto apply() const
  {
    return ApplyTo<>{};
  }
};

template<template<typename...> typename... MetaTypes>
struct metatypelist
{
  template<typename... Types>
  constexpr auto establish() const
  {
    return typelist<MetaTypes<Types...>...>{};
  }
};

template<template<size_t...> typename... MetaTypes>
struct metaindexrange
{
  template<size_t... Indices>
  constexpr auto establish() const
  {
    return typelist<MetaTypes<Indices...>...>{};
  }
};

template<typename... Ts>
struct typelist
{
  using head_type = typename tl_helper<Ts...>::head_type;
  using next_types = typename tl_helper<Ts...>::next_types;

  const size_t size = sizeof...(Ts);
  const size_t sizeof_min = (sizeof(head_type) < next_types{}.sizeof_min) ? sizeof(head_type) : next_types{}.sizeof_min;
  const size_t sizeof_max = (sizeof(head_type) > next_types{}.sizeof_min) ? sizeof(head_type) : next_types{}.sizeof_max;
  const size_t alignof_min =
    (alignof(head_type) < next_types{}.alignof_min) ? alignof(head_type) : next_types{}.alignof_min;
  const size_t alignof_max =
    (alignof(head_type) > next_types{}.alignof_min) ? alignof(head_type) : next_types{}.alignof_max;

  const size_t end_index = sizeof...(Ts) - 1;
  const size_t invlaid_index = sizeof...(Ts);

  template<size_t Begin, size_t End, size_t Index, typename... Collector>
  constexpr auto subrange() const
  {
    if constexpr (Index >= Begin && Index <= End) {
      return subrange<Begin, End, Index + 1, Collector..., typename ct_select<Index, Ts...>::type>();
    } else if constexpr (Index < Begin) {
      return subrange<Begin, End, Index + 1>();
    } else if constexpr (Index > End) {
      return typelist<Collector...>{};
    }
  }

  template<size_t Index, template<typename...> typename TL, typename... Deduced>
  constexpr auto splice(TL<Deduced...>) const
  {
    constexpr auto first_partition = subrange<0, Index - 1>();
    constexpr auto last_partition = subrange<Index, end_index>();
    return first_partition.append_types(TL<Deduced...>{}).join(last_partition);
  }

  template<template<typename...> typename TL, typename... Deduced>
  constexpr auto append(TL<Deduced...>) const
  {
    return typelist<Ts..., Deduced...>{};
  }

  template<template<typename...> typename TL, typename... Deduced>
  constexpr auto prepend(TL<Deduced...>) const
  {
    return typelist<Deduced..., Ts...>{};
  }

  template<size_t Index>
  constexpr auto erase(const std::integral_constant<size_t, Index>) const
  {
    constexpr auto first_partition = subrange<0, Index - 1>();
    constexpr auto last_partition = subrange<Index + 1, end_index>();
    return first_partition.join(last_partition);
  }

  template<size_t Begin, size_t End>
  constexpr auto erase(const std::integral_constant<size_t, Begin>, const std::integral_constant<size_t, End>) const
  {
    constexpr auto first_partition = subrange<0, Begin - 1>();
    constexpr auto last_partition = subrange<End + 1, end_index>();
    return first_partition.join(last_partition);
  }

  template<template<typename...> typename ApplyTo>
  constexpr auto apply() const
  {
    return ApplyTo<Ts...>{};
  }

  template<typename Type>
  constexpr auto index_of() const
  {
    return index_of<Type, Ts...>();
  }

  template<size_t Index>
  constexpr auto type_at(const std::integral_constant<size_t, Index>) const
  {
    return select_t<Index, Ts...>{};
  }

  template<size_t Index>
  constexpr auto operator[](const std::integral_constant<size_t, Index>) const
  {
    return type_at(std::integral_constant<size_t, Index>{});
  }

  constexpr auto front() const { return select_t<0, Ts...>{}; }

  constexpr auto back() const { return select_t<end_index, Ts...>{}; }
};
}