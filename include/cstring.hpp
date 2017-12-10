#pragma once
#include "citerator.hpp"
#include "cstring_operators.hpp"
#include "utility.hpp"

namespace utility::cexpr {

template<char... Chars>
struct cstring
{
  static constexpr size_t size = sizeof...(Chars);
  const char data[size + 1] = { Chars..., '\0' };

  constexpr auto begin() const { return citerator<cstring<Chars...>, 0>{}; }
  constexpr auto end() const { return citerator<cstring<Chars...>, sizeof...(Chars)>{}; }
};

template<template<auto...> typename String, typename Literal>
struct builder
{
  template<size_t... Indices>
  struct build
  {
    typedef String<Literal{}.data[Indices]...> result;
  };
};

template<size_t Count, template<size_t...> class Meta, size_t... Indices>
struct apply_range
{
  typedef typename apply_range<Count - 1, Meta, Count - 1, Indices...>::result result;
};

template<template<size_t...> class Meta, size_t... Indices>
struct apply_range<0, Meta, Indices...>
{
  typedef typename Meta<Indices...>::result result;
};
}

#ifdef USING_CSTRING
#define CSTRING(literal)                                                                                               \
  [] {                                                                                                                 \
    struct literal_type                                                                                                \
    {                                                                                                                  \
      const char* data = literal;                                                                                      \
    };                                                                                                                 \
    return utility::cstring::apply_range<                                                                              \
      sizeof(literal) - 1,                                                                                             \
      utility::cstring::builder<utility::cstring::cstring, literal_type>::build>::result{};                            \
  }()
#endif
