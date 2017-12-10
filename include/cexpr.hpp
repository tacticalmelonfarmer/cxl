#ifndef CEXPR_HPP_GAURD
#define CEXPR_HPP_GAURD
#include "typelist.hpp"
#include "utility.hpp"
#include <assert.h>
#include <functional>

namespace utility::cexpr {
using namespace ct;

///////////////////////    citerator     ///////////////////////////////////

template<size_t Index, typename Container, size_t Begin, size_t End>
struct citerator
{
  typedef Container container_type;
  typedef typename container_type::value_type value_type;

  const container_type container;

  constexpr citerator(const Container Init)
    : container(Init)
  {}

  constexpr auto operator*() const { return container[Index]; }
  constexpr size_t index() const { return Index; }
  constexpr auto operator++() const { return citerator<Index + 1, Container, End, Begin>(container); }
  constexpr auto operator--() const { return citerator<Index - 1, Container, End, Begin>(container); }

  template<template<size_t> typename T, size_t... Indices>
  constexpr auto until_helper(const index_range<Indices...>) const
  {
    return T<sizeof...(Indices)>{ { container[Indices]... } };
  }
  template<template<size_t> typename T,                       // container type == cstring<...>
           template<size_t, typename, auto...> typename Iter, // deduced
           size_t UntilIndex,                                 // deduced
           auto... __unused>                                  // deduced
  constexpr auto
  until(const Iter<UntilIndex, Container, __unused...> Until) const
  {
    return until_helper<T>(make_index_range_t<Index, UntilIndex>());
  }
  template<template<typename, size_t> typename T, size_t... Indices>
  constexpr auto until_helper(const index_range<Indices...>) const
  {
    return T<value_type, sizeof...(Indices)>{ { container[Indices]... } };
  }
  template<template<typename, size_t> typename T,             // container type == carray<...>
           template<size_t, typename, auto...> typename Iter, // deduced
           size_t UntilIndex,                                 // deduced
           auto... __unused>                                  // deduced
  constexpr auto
  until(const Iter<UntilIndex, Container, __unused...> Until) const
  {
    return until_helper<T>(make_index_range_t<Index, UntilIndex>());
  }
};

template<template<size_t> typename T, // cstring
         template<size_t, typename, size_t, size_t> typename Iter,
         size_t From,
         size_t To,
         typename Container,
         size_t Begin,
         size_t End>
constexpr auto
from_iters(const Iter<From, Container, Begin, End> BeginIter, const Iter<To, Container, Begin, End> EndIter)
{
  return BeginIter.template until<T>(EndIter);
}

template<template<typename, size_t> typename T, // carray
         template<size_t, typename, size_t, size_t> typename Iter,
         size_t From,
         size_t To,
         typename Container,
         size_t Begin,
         size_t End>
constexpr auto
from_iters(const Iter<From, Container, Begin, End> BeginIter, const Iter<To, Container, Begin, End> EndIter)
{
  return BeginIter.template until<T>(EndIter);
}

template<typename T, typename U, typename V, typename W>
constexpr auto
iter_compare(const T Begin1, const U End1, const V Begin2, const W End2)
{
  if (*Begin1 == *Begin2)
    return iter_compare(++Begin1, End1, ++Begin2, End2);
  else
    return Begin1;
}

///////////////////////    carray     ///////////////////////////////////

template<typename T, size_t ArrSize>
struct carray
{
  typedef T value_type;

  const T data[ArrSize];

  constexpr carray(const T Data)
    : data{ Data }
  {}
  template<typename T0, typename... Ts>
  constexpr carray(T0 Data0, Ts... Data)
    : data{ Data0, Data... }
  {}
  template<size_t Size, size_t... Indices>
  constexpr carray(const index_range<Indices...>, const T (&Data)[Size])
    : data{ Data[Indices]... }
  {}
  template<size_t Size>
  constexpr carray(const T (&Data)[Size])
    : carray(make_index_range_t<0, Size - 1>(), Data)
  {}

  constexpr T operator[](const size_t Index) const { return data[Index]; }
  constexpr size_t size() const { return ArrSize; }
};

template<typename T, size_t Size>
carray(const T (&Data)[Size])->carray<T, Size>;

template<typename T0, typename... Ts>
carray(T0 Data0, Ts... Data)->carray<T0, sizeof...(Ts) + 1>;

template<typename T>
carray(const T)->carray<T, 1>;

///////////////////////    cstring     ///////////////////////////////////

constexpr auto
idchar(const char Ch, const size_t Count)
{
  return Ch;
}

template<typename T, size_t... IndicesL>
constexpr auto
fill_string(const index_range<IndicesL...>)
{
  return T{ { idchar('\000', IndicesL)... } };
}

template<size_t StrSize>
struct cstring
{
  typedef char value_type;

  const char data[StrSize];

  constexpr cstring(const char Ch)
    : data{ Ch }
  {}
  template<size_t Size, size_t... Indices>
  constexpr cstring(const char (&Data)[Size], const index_range<Indices...>)
    : data{ Data[Indices]... }
  {}
  template<size_t Size>
  constexpr cstring(const char (&Data)[Size])
    : cstring(Data, make_index_range_t<0, Size - 2>())
  {}
  constexpr cstring()
    : cstring(fill_string<cstring<StrSize>>(make_index_range_t<0, StrSize - 1>()))
  {}

  constexpr char operator[](const size_t Index) const { return data[Index]; }
  constexpr size_t size() const { return StrSize; }
  constexpr auto begin() const { return citerator<0, cstring<StrSize>, 0, StrSize - 1>(*this); }
  constexpr auto end() const { return citerator<StrSize - 1, cstring<StrSize>, 0, StrSize - 1>(*this); }

  template<size_t... Indices>
  constexpr auto substr_helper(const index_range<Indices...>) const
  {
    return cstring<sizeof...(Indices)>{ { data[Indices]... } };
  }
  template<size_t Begin, size_t End>
  constexpr auto substr() const
  {
    return substr_helper(make_index_range_t<Begin, End>());
  }
};

template<size_t Size>
cstring(const char (&Data)[Size])->cstring<Size - 1>;

cstring(const char Ch)->cstring<1>;

template<typename T, size_t SizeL, size_t SizeR, size_t... IndicesL, size_t... IndicesR>
constexpr auto
array_concat_helper(const T (&L)[SizeL],
                    const T (&R)[SizeR],
                    const index_range<IndicesL...>,
                    const index_range<IndicesR...>)
{
  if constexpr (std::is_same_v<T, char>)
    return cstring<SizeL + SizeR>{ { L[IndicesL]..., R[IndicesR]..., '\000' } };
  else
    return carray<T, SizeL + SizeR>{ { L[IndicesL]..., R[IndicesR]... } };
}

template<template<size_t> typename T, size_t SizeL, size_t SizeR>
constexpr auto
operator+(const T<SizeL> L, const T<SizeR> R)
{
  return array_concat_helper(
    L.data, R.data, make_index_range_t<0, L.size() - 1>(), make_index_range_t<0, R.size() - 1>());
}

template<template<size_t> typename T, typename U, size_t SizeL, size_t SizeR>
constexpr auto
operator+(const T<SizeL> L, const U (&R)[SizeR])
{
  return array_concat_helper(L.data, R, make_index_range_t<0, L.size() - 1>(), make_index_range_t<0, R.size() - 1>());
}

template<template<size_t> typename T, typename U, size_t SizeL, size_t SizeR>
constexpr auto
operator+(const U (&L)[SizeL], const T<SizeR> R)
{
  return array_concat_helper(L, R.data, make_index_range_t<0, L.size() - 1>(), make_index_range_t<0, R.size() - 1>());
}

template<typename T, size_t Size>
constexpr bool
array_match_helper(const size_t Index, const T (&L)[Size], const T (&R)[Size])
{
  if (Index >= Size)
    return true;
  else
    return L[Index] == R[Index] && array_match_helper(Index + 1, L, R);
}

template<template<size_t> typename T, size_t Size>
constexpr bool
operator==(const T<Size> L, const T<Size> R)
{
  return array_match_helper(0, L.data, R.data);
}

template<typename T, size_t SizeL, size_t SizeR>
constexpr size_t
array_compare(const size_t Index, const T (&L)[SizeL], const T (&R)[SizeR])
{
  if (Index >= SizeL || Index >= SizeR || L[Index] != R[Index])
    return Index;
  else if (L[Index] == R[Index])
    return array_compare(Index + 1, L, R);
  else
    return 0;
}

///////////////////////    ctuple     ///////////////////////////////////

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

///////////////////////    parser stuff     ///////////////////////////////////

template<typename T0, typename... Ts>
struct one_of_strings
{
  typedef typelist<T0, Ts...> target_types;
  typedef size_t yield_type;

  const size_t target_count;
  const ctuple<T0, Ts...> targets;

  constexpr one_of_strings(const T0 Target0, const Ts... Targets)
    : target_count(sizeof...(Ts) + 1)
    , targets{ Target0, Targets... }
  {}

  constexpr one_of_strings()
    : target_count(sizeof...(Ts) + 1)
    , targets{ T0(), Ts()... }
  {}

  template<size_t Index, typename U, typename... Vs, typename T>
  constexpr size_t parse_impl(const T Subject) const
  {
    size_t match_size = array_compare(0, Subject.data, targets.template get<Index>().data);
    bool matched = match_size == targets.template get<Index>().size();
    if (matched)
      return match_size;
    else if (Index >= targets.template get<Index>().size())
      return 0;
    else if constexpr (sizeof...(Vs) > 0)
      return parse_impl<Index + 1, Vs...>(Subject);
    else
      return 0;
  }
  template<typename T>
  constexpr size_t parse(const T Subject) const
  {
    size_t match_size = array_compare(0, Subject.data, targets.template get<0>().data);
    bool matched = match_size == targets.template get<0>().size();
    if (matched)
      return match_size;
    else if constexpr (sizeof...(Ts) > 0)
      return parse_impl<1, Ts...>(Subject);
    else
      return 0;
  }

  template<template<size_t, typename, size_t, size_t> typename Iter,
           size_t From,
           size_t To,
           typename T,
           size_t __unused1,
           size_t __unused2>
  constexpr auto iter_parse(const Iter<From, T, __unused1, __unused2> BeginIter,
                            const Iter<To, T, __unused1, __unused2> EndIter) const
  {
    auto current_target = targets.template get<0>();
    auto matched = iter_compare(BeginIter, EndIter, current_target.begin(), current_target.end());
    return matched;
  }
};

template<typename P0, typename... Ps>
struct one_of_parsers
{
  typedef typelist<P0, Ps...> target_types;
  typedef size_t yield_type;

  const size_t parser_count;
  const ctuple<P0, Ps...> parsers;

  constexpr one_of_parsers(const P0 Parser0, const Ps... Parsers)
    : parser_count(sizeof...(Ps) + 1)
    , parsers{ Parser0, Parsers... }
  {}

  constexpr one_of_parsers()
    : parser_count(sizeof...(Ps) + 1)
    , parsers{ P0(), Ps()... }
  {}

  template<size_t Index, typename U, typename... Vs, typename T>
  constexpr size_t parse_impl(const T Subject) const
  {
    auto current_parser = parsers.template get<Index>();
    size_t matched = current_parser.parse(Subject);
    if (matched)
      return matched;
    else if constexpr (sizeof...(Vs) > 0)
      return parse_impl<Index + 1, Vs...>(Subject);
    else
      return 0;
  }
  template<typename T>
  constexpr size_t parse(const T Subject) const
  {
    auto current_parser = parsers.template get<0>();
    size_t matched = current_parser.parse(Subject);
    if (matched)
      return matched;
    else if constexpr (sizeof...(Ps) > 0)
      return parse_impl<1, Ps...>(Subject);
    else
      return 0;
  }
};

template<size_t Size0, size_t... Sizes>
struct seq_strings
{
  typedef typelist<cstring<Size0>, cstring<Sizes>...> target_types;
  typedef size_t yield_type;

  const size_t target_count;
  const ctuple<cstring<Size0>, cstring<Sizes>...> targets;

  constexpr seq_strings(const cstring<Size0> Target0, const cstring<Sizes>... Targets)
    : target_count(sizeof...(Sizes) + 1)
    , targets{ Target0, Targets... }
  {}

  constexpr seq_strings()
    : target_count(sizeof...(Sizes) + 1)
    , targets{ cstring<Size0>(), cstring<Sizes>()... }
  {}

  template<size_t Index, size_t Bump0, size_t... Bumps, size_t Size>
  constexpr size_t parse_impl(const cstring<Size> Subject, const size_t Result) const
  {
    size_t matched = array_compare(0, Subject.template substr<0, Bump0 - 1>().data, targets.template get<Index>().data);
    if (matched) {                         // match succesful
      if constexpr (sizeof...(Bumps) == 0) // target_count == 1
        return Result + Bump0;
      else // multiple targets
        return parse_impl<Index + 1, Bumps...>(Subject.template substr<Bump0, Size - 1>(), Result + Bump0);
    } else // match failure
      return 0;
  }

  template<size_t Size>
  constexpr size_t parse(const cstring<Size> Subject) const
  {
    size_t matched = array_compare(0, Subject.template substr<0, Size0 - 1>().data, targets.template get<0>().data);
    if (matched) {                         // match succesful
      if constexpr (sizeof...(Sizes) == 0) // target_count == 1
        return Size0;
      else // multiple targets
        return parse_impl<1, Sizes...>(Subject.template substr<Size0, Size - 1>(), Size0);
    } else // match failure
      return 0;
  }
};

template<typename P0, typename... Ps>
struct seq_parsers
{
  typedef typelist<P0, Ps...> target_types;
  typedef size_t yield_type;

  const size_t parser_count;
  const ctuple<P0, Ps...> parsers;

  constexpr seq_parsers(const P0 Parser0, const Ps... Parsers)
    : parser_count(sizeof...(Ps) + 1)
    , parsers{ Parser0, Parsers... }
  {}

  constexpr seq_parsers()
    : parser_count(sizeof...(Ps) + 1)
    , parsers{ P0(), Ps()... }
  {}

  template<size_t Index, size_t Size, size_t... Sizes>
  constexpr size_t parse_impl(const size_t Result, const cstring<Size> Subject, const cstring<Sizes>... Next) const
  {
    auto current_parser = parsers.template get<Index>();
    size_t matched = current_parser.parse(Subject);
    if (matched) {
      if (sizeof...(Next) == 0)
        return Result + Size;
      else if constexpr (sizeof...(Next) > 0)
        return parse_impl<Index + 1>(Result + Size, Next...);
    } else
      return 0;
  }

  template<size_t Size, size_t... Sizes>
  constexpr size_t parse(const cstring<Size> Subject, const cstring<Sizes>... Next) const
  {
    auto current_parser = parsers.template get<0>();
    size_t matched = current_parser.parse(Subject);
    if (matched) {
      if constexpr (sizeof...(Ps) == 0)
        return Size;
      else
        return parse_impl<1>(matched, Next...);
    } else
      return 0;
  }
};

template<typename P, typename S, typename F>
struct if_call
{
  typedef P predicate_type;

  const P predicate;
  const S yield_success;
  const F yield_failure;

  constexpr if_call(const P Predicate, const S YieldSuccess, const F YieldFailure)
    : predicate(Predicate)
    , yield_success(YieldSuccess)
    , yield_failure(YieldFailure)
  {}

  template<typename T>
  constexpr auto parse(const T Subject) const
  {
    return predicate.parse(Subject) ? yield_success : yield_failure;
  }
};
}
#endif