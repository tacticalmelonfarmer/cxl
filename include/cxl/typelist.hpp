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
struct typelist_info;

template<>
struct typelist_info<>
{
  using head_type = void;
  using next_types = void;
};

template<typename T0, typename... Ts>
struct typelist_info<T0, Ts...>
{
  using head_type = T0;
  using next_types = typelist<Ts...>;
};

template<template<typename...> typename... MetaTypes>
struct metatypelist
{
  template<template<typename> typename TL, typename... Types>
  constexpr auto establish_one_for_each(TL<Types...>) const
  {
    return typelist<MetaTypes<Types>...>{};
  }
  template<template<typename> typename TL, typename... Types>
  constexpr auto establish_all_for_each(TL<Types...>) const
  {
    return typelist<MetaTypes<Types...>...>{};
  }
};

template<template<index_t...> typename... MetaTypes>
struct metaindexrange
{
  template<template<index_t> typename IS, index_t... Indices>
  constexpr auto establish_one_for_each(IS<Indices...>) const
  {
    return typelist<MetaTypes<Indices>...>{};
  }
  template<template<index_t> typename IS, index_t... Indices>
  constexpr auto establish_all_for_each(IS<Indices...>) const
  {
    return typelist<MetaTypes<Indices...>...>{};
  }
};

template<typename T>
struct emplacer
{
  constexpr emplacer() {}
  template<typename... ArgTs>
  constexpr T operator()(ArgTs&&... arguments) const
  {
    return T(std::forward<ArgTs>(arguments)...);
  }
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

  struct error
  {};

  template<index_t Begin, index_t End>
  constexpr auto subrange() const
  {
    static_assert(false, "cannot get subrange of empty typelist");
    return error{};
  }

  template<index_t Index, template<typename...> typename TL, typename... Deduced>
  constexpr auto insert(TL<Deduced...>) const
  {
    static_assert(Index == 0, "inserting into empty typelists can only be done at index 0");
    return typelist<Deduced...>{};
  }

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

  template<typename Error>
  constexpr Error erase(...) const
  {
    static_assert(false, "cannot erase types from an empty typelist");
    return error{};
  }

  template<template<typename...> typename ApplyTo>
  constexpr auto applied_emplacer() const
  {
    return emplacer<ApplyTo<>>{};
  }

  template<typename Type, typename Error>
  constexpr Error index_of() const
  {
    static_assert(false, "no types to get index of in empty typelist");
    return error{};
  }

  template<typename Error>
  constexpr Error type_emplacer(...) const
  {
    static_assert(false, "no types in empty typelist to emplace");
    return error{};
  }

  template<typename... U>
  constexpr auto operator[](U...) const
  {
    return type_emplacer();
  }

  template<typename Error>
  constexpr Error front() const
  {
    static_assert(false, "empty typelist has no front");
    return error{};
  }

  template<typename Error>
  constexpr Error back() const
  {
    static_assert(false, "empty typelist has no back");
    return error{};
  }

  constexpr auto begin() const { return iterator<typelist<>, 1>{}; }
  constexpr auto end() const { return iterator<typelist<>, 1>{}; }
};

template<typename... Ts>
struct typelist
{
  using head_type = typename typelist_info<Ts...>::head_type;
  using next_types = typename typelist_info<Ts...>::next_types;

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
  constexpr auto insert(TL<Deduced...>) const
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
  constexpr auto applied_emplacer() const
  {
    return emplacer<ApplyTo<Ts...>>{};
  }

  template<typename Type>
  constexpr auto index_of() const
  {
    return index_of<Type, Ts...>();
  }

  template<index_t Index>
  constexpr auto type_emplacer(const std::integral_constant<index_t, Index>) const
  {
    return emplacer<select_t<Index, Ts...>>{};
  }

  template<index_t Index>
  constexpr auto operator[](const std::integral_constant<index_t, Index>) const
  {
    return type_emplacer(std::integral_constant<index_t, Index>{});
  }

  constexpr auto front() const { return select_t<0, Ts...>{}; }
  constexpr auto back() const { return select_t<m_end_index - 1, Ts...>{}; }

  constexpr auto begin() const { return iterator<typelist<Ts...>, 0>{}; }
  constexpr auto end() const { return iterator<typelist<Ts...>, m_end_index>{}; }

private:
  const index_t m_end_index = sizeof...(Ts);
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