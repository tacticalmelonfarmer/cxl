#pragma once
#include "citerator.hpp"
#include "utility.hpp"

namespace utility::cexpr {

template<char...>
struct cstring;

namespace literals {
template<class Char, Char... Digits>
constexpr auto operator"" _cs() -> cstring<Digits...>
{
  return {};
}
}

template<char... L, char... R>
constexpr cstring<L..., R...>
operator+(const cstring<L...>&, const cstring<R...>&)
{
  return cstring<L..., R...>{};
}

template<typename String, size_t... Indices>
constexpr auto
substr(const String& str, const ct::index_range<Indices...>)
{
  return cstring<decltype(str){}.data[Indices]...>{};
}

template<size_t Begin, size_t End, typename String>
constexpr auto
substr(const String& str)
{
  static_assert(Begin < End);
  return substr(str, ct::make_index_range_t<Begin, End>());
}

template<typename String, size_t Begin, size_t End>
constexpr auto
substr(const citerator<String, Begin>&, const citerator<String, End>&)
{
  return substr<Begin, End>(String{});
}
}