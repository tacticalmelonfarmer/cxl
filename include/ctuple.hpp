#pragma once
#include "typelist.hpp"
#include "utility.hpp"

namespace utility::cexpr {
template<typename T, typename N>
struct ctuple_element
{
  typedef N next_type;
  typedef T value_type;

  const size_t index;
  const T value;
  const next_type next;

  constexpr ctuple_element(const size_t Index, const T Value, const N Next)
    : index(Index)
    , value(Value)
    , next(Next)
  {}
};

template<typename T>
struct ctuple_element<T, int>
{
  typedef int next_type;
  typedef T value_type;

  const size_t index;
  const T value;
  const int next;

  constexpr ctuple_element(const size_t Index, const T Value, const int Next)
    : index(Index)
    , value(Value)
    , next(Next)
  {}
};

template<typename T0>
constexpr auto
construct_element(const size_t Index, const T0 Elem0)
{
  return ctuple_element(Index, Elem0, int(0));
}

template<typename T0, typename... Ts>
constexpr auto
construct_element(const size_t Index, const T0 Elem0, const Ts... Elems)
{
  return ctuple_element(Index, Elem0, construct_element(Index + 1, Elems...));
}

template<typename T0, typename... Ts>
struct ctuple
{
  typedef typelist<T0, Ts...> types;
  typedef decltype(construct_element(0, T0(), Ts()...)) container_type;

  const container_type first_element;

  constexpr ctuple(const T0 Elem0, const Ts... Elems)
    : first_element(construct_element(0, Elem0, Elems...))
  {}

  struct bad_tuple_access
  {};

  template<size_t Index, typename T>
  constexpr auto get_(const T Elem) const
  {
    if constexpr (Index == 0)
      return Elem.value;
    else if constexpr (!std::is_same_v<decltype(Elem.next), int>)
      return get_<Index - 1>(Elem.next);
    else
      return bad_tuple_access{};
  }
  template<size_t Index>
  constexpr auto get() const
  {
    return get_<Index>(first_element);
  }
};
}