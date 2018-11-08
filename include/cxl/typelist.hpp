#pragma once

#include "integral.hpp"
#include "iterator.hpp"
#include "utility.hpp"
#include <type_traits>

namespace cxl {

template<typename... Ts>
struct typelist;

inline namespace detail {

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

template<>
struct typelist<>
{
  using head_type = void;
  using next_types = void;

  constexpr auto size() const { return std::integral_constant<index_t, 0>{}; }
  constexpr auto largest_alignment() const { return std::integral_constant<index_t, 0>{}; }
  constexpr auto smallest_alignment() const { return std::integral_constant<index_t, 0>{}; }
  constexpr auto largest_size() const { return std::integral_constant<index_t, 0>{}; }
  constexpr auto smallest_size() const { return std::integral_constant<index_t, 0>{}; }

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

template<template<index_t...> typename... MetaTypes>
struct metaindexrange
{
  template<index_t... Indices>
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

  constexpr auto size() const { return std::integral_constant<index_t, sizeof...(Ts)>{}; }
  constexpr auto largest_alignment() const
  {
    return std::integral_constant < index_t, (alignof(head_type) > next_types{}.smallest_alignment())
                                               ? alignof(head_type)
                                               : next_types{}.largest_alignment() > {};
  }
  constexpr auto smallest_alignment() const
  {
    return std::integral_constant < index_t, (alignof(head_type) < next_types{}.smallest_alignment())
                                               ? alignof(head_type)
                                               : next_types{}.smallest_alignment() > {};
  }
  constexpr auto largest_size() const
  {
    return std::integral_constant < index_t,
           (sizeof(head_type) > next_types{}.smallest_size()) ? sizeof(head_type) : next_types{}.largest_size() > {};
  }
  constexpr auto smallest_size() const
  {
    return std::integral_constant < index_t,
           (sizeof(head_type) < next_types{}.smallest_size()) ? sizeof(head_type) : next_types{}.smallest_size() > {};
  }

  template<index_t Begin, index_t End, index_t Index, typename... Collector>
  constexpr auto subrange() const
  {
    if constexpr (Index >= Begin && Index <= End) {
      return subrange<Begin, End, Index + 1, Collector..., select_t<Index, Ts...>>();
    } else if constexpr (Index < Begin) {
      return subrange<Begin, End, Index + 1>();
    } else if constexpr (Index > End) {
      return typelist<Collector...>{};
    }
  }

  template<index_t Index, template<typename...> typename TL, typename... Deduced>
  constexpr auto splice(TL<Deduced...>) const
  {
    constexpr auto first_partition = subrange<0, Index - 1>();
    constexpr auto last_partition = subrange<Index, m_end_index>();
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

  template<index_t Index>
  constexpr auto erase(const std::integral_constant<index_t, Index>) const
  {
    constexpr auto first_partition = subrange<0, Index - 1>();
    constexpr auto last_partition = subrange<Index + 1, m_end_index>();
    return first_partition.join(last_partition);
  }

  template<index_t Begin, index_t End>
  constexpr auto erase(const std::integral_constant<index_t, Begin>, const std::integral_constant<index_t, End>) const
  {
    constexpr auto first_partition = subrange<0, Begin - 1>();
    constexpr auto last_partition = subrange<End + 1, m_end_index>();
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

  template<index_t Index>
  constexpr auto type_at(const std::integral_constant<index_t, Index>) const
  {
    return select_t<Index, Ts...>{};
  }

  template<index_t Index>
  constexpr auto operator[](const std::integral_constant<index_t, Index>) const
  {
    return type_at(std::integral_constant<index_t, Index>{});
  }

  constexpr auto front() const { return select_t<0, Ts...>{}; }
  constexpr auto back() const { return select_t<m_end_index, Ts...>{}; }

  constexpr auto begin() const { return iterator<typelist<Ts...>, 0>{}; }
  constexpr auto end() const { return iterator<typelist<Ts...>, sizeof...(Ts)>{}; }

private:
  const index_t m_end_index = sizeof...(Ts) - 1;
  const index_t m_invalid_index = sizeof...(Ts);
};

template<typename... Types>
constexpr bool
is_typelist(typelist<Types...>)
{
  return true;
}

constexpr bool
is_typelist(...)
{
  return false;
}
}