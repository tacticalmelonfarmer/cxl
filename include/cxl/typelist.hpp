#pragma once

#include "integral.hpp"
#include "iterator.hpp"
#include "utility.hpp"
#include <type_traits>

namespace cxl
{

template <typename... Ts>
struct typelist;

inline namespace detail
{

template <typename... Ts>
struct typelist_info;

template <template <typename...> typename... MetaTypes>
struct metatypelist
{
  template <template <typename...> typename TL, typename... Types>
  constexpr auto establish_one_for_each(TL<Types...>) const
  {
    return typelist<MetaTypes<Types>...>{};
  }
  template <template <typename...> typename TL, typename... Types>
  constexpr auto establish_all_for_each(TL<Types...>) const
  {
    return typelist<MetaTypes<Types...>...>{};
  }
};

template <template <index_t...> typename... MetaTypes>
struct metaindexrange
{
  template <template <index_t> typename IS, index_t... Indices>
  constexpr auto establish_one_for_each(IS<Indices...>) const
  {
    return typelist<MetaTypes<Indices>...>{};
  }
  template <template <index_t> typename IS, index_t... Indices>
  constexpr auto establish_all_for_each(IS<Indices...>) const
  {
    return typelist<MetaTypes<Indices...>...>{};
  }
};

template <typename T>
struct emplacer
{
  constexpr emplacer() {}
  template <typename... ArgTs>
  constexpr T operator()(ArgTs &&... arguments) const
  {
    return T(::std::forward<ArgTs>(arguments)...);
  }
};
} // namespace detail

template <>
struct typelist<>
{
  constexpr auto size() const { return ::std::integral_constant<index_t, 0>{}; }
  constexpr auto largest_alignment() const { return ::std::integral_constant<index_t, 0>{}; }
  constexpr auto smallest_alignment() const { return ::std::integral_constant<index_t, 0>{}; }
  constexpr auto largest_size() const { return ::std::integral_constant<index_t, 0>{}; }
  constexpr auto smallest_size() const { return ::std::integral_constant<index_t, 0>{}; }

  template <template <typename...> typename TL, typename... Deduced>
  constexpr auto append(TL<Deduced...>) const
  {
    return typelist<Deduced...>{};
  }
};

template <typename T0, typename... Ts>
struct typelist<T0, Ts...>
{
  using head_type = T0;
  using next_types = typelist<Ts...>;

  constexpr auto size() const { return ::std::integral_constant<index_t, sizeof...(Ts) + 1>{}; }
  constexpr auto largest_alignment() const
  {
    return ::std::integral_constant < index_t, (alignof(head_type) > next_types{}.smallest_alignment())
                                                   ? alignof(head_type)
                                                   : next_types{}.largest_alignment() > {};
  }
  constexpr auto smallest_alignment() const
  {
    return ::std::integral_constant < index_t, (alignof(head_type) < next_types{}.smallest_alignment())
                                                   ? alignof(head_type)
                                                   : next_types{}.smallest_alignment() > {};
  }
  constexpr auto largest_size() const
  {
    return ::std::integral_constant < index_t,
           (sizeof(head_type) > next_types{}.smallest_size()) ? sizeof(head_type) : next_types{}.largest_size() > {};
  }
  constexpr auto smallest_size() const
  {
    return ::std::integral_constant < index_t,
           (sizeof(head_type) < next_types{}.smallest_size()) ? sizeof(head_type) : next_types{}.smallest_size() > {};
  }

  template <index_t Begin, index_t End, index_t Index, typename... Collector>
  constexpr auto subrange() const
  {
    static_assert(Begin >= 0, "invalid Begin index for typelist<...>::subrange");
    static_assert(End <= m_end_index, "invalid End index for typelist<...>::subrange");
    if constexpr (Index >= Begin && Index <= End)
    {
      return subrange<Begin, End, Index + 1, Collector..., select_t<Index, T0, Ts...>>();
    }
    else if constexpr (Index < Begin)
    {
      return subrange<Begin, End, Index + 1>();
    }
    else if constexpr (Index > End)
    {
      return typelist<Collector...>{};
    }
  }

  template <index_t Index, template <typename...> typename TL, typename... Deduced>
  constexpr auto insert(TL<Deduced...>) const
  {
    constexpr auto first_partition = subrange<0, Index - 1>();
    constexpr auto last_partition = subrange<Index, m_end_index>();
    return first_partition.append(TL<Deduced...>{}).join(last_partition);
  }

  template <template <typename...> typename TL, typename... Deduced>
  constexpr auto append(TL<Deduced...>) const
  {
    return typelist<T0, Ts..., Deduced...>{};
  }

  template <template <typename...> typename TL, typename... Deduced>
  constexpr auto prepend(TL<Deduced...>) const
  {
    return typelist<Deduced..., T0, Ts...>{};
  }

  template <index_t Index>
  constexpr auto erase(::std::integral_constant<index_t, Index>) const
  {
    constexpr auto first_partition = subrange<0, Index - 1>();
    constexpr auto last_partition = subrange<Index + 1, m_end_index>();
    return first_partition.join(last_partition);
  }

  template <index_t Begin, index_t End>
  constexpr auto erase(::std::integral_constant<index_t, Begin>, ::std::integral_constant<index_t, End>) const
  {
    constexpr auto first_partition = subrange<0, Begin - 1>();
    constexpr auto last_partition = subrange<End + 1, m_end_index>();
    return first_partition.join(last_partition);
  }

  template <typename BeginIter, typename EndIter>
  constexpr auto erase(BeginIter, EndIter) const
  {
    constexpr auto first_partition = subrange<0, BeginIter{}.index() - 1>();
    constexpr auto last_partition = subrange<EndIter{}.index() + 1, m_end_index>();
    return first_partition.join(last_partition);
  }

  template <template <typename...> typename ApplyTo>
  constexpr auto applied_emplacer() const
  {
    return emplacer<ApplyTo<T0, Ts...>>{};
  }

  template <typename Type>
  constexpr auto index_of() const
  {
    return index_of<Type, T0, Ts...>();
  }

  template <index_t Index>
  constexpr auto type_emplacer(::std::integral_constant<index_t, Index>) const
  {
    return emplacer<select_t<Index, T0, Ts...>>{};
  }

  template <index_t Index>
  constexpr auto operator[](::std::integral_constant<index_t, Index>) const
  {
    return type_emplacer(::std::integral_constant<index_t, Index>{});
  }

  constexpr auto front() const { return emplacer<select_t<0, T0, Ts...>>{}; }
  constexpr auto back() const { return emplacer<select_t<m_end_index - 1, T0, Ts...>>{}; }

  constexpr auto begin() const { return iterator<typelist<T0, Ts...>, 0>{}; }
  constexpr auto end() const { return iterator<typelist<T0, Ts...>, m_end_index>{}; }

private:
  static constexpr index_t m_end_index = sizeof...(Ts) + 1;
};

template <typename... Types>
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
} // namespace cxl