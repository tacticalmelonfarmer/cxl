#pragma once
#include "citerator.hpp"
#include "utility.hpp"

namespace utility::cexpr {

template<char... Chars>
struct cstring
{
  typedef char value_type;

  const cindex<sizeof...(Chars)> size = {};
  const char data[sizeof...(Chars) + 1] = { Chars..., '\0' };

  constexpr operator const char*() const { return &data[0]; }
  constexpr char operator[](size_t index) const { return data[index]; }
  constexpr char front() const { return data[0]; }
  constexpr char back() const { return data[sizeof...(Chars) - 1]; }

  constexpr auto begin() const { return citerator<cstring<Chars...>, 0>{}; }
  constexpr auto end() const { return citerator<cstring<Chars...>, sizeof...(Chars)>{}; }
};

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
substr(const String&)
{
  constexpr auto string = String{};
  if constexpr (Begin == End)
    return cstring<string.back()>{};
  else if constexpr (Begin < End)
    return substr(string, ct::make_index_range_t<Begin, End>());
  else {
    struct cstring_substr_compile_time_failure
    {};
    return cstring_substr_compile_time_failure{};
  }
}

template<typename String, size_t Begin, size_t End>
constexpr auto
substr(const citerator<String, Begin>&, const citerator<String, End>& end)
{
  if constexpr (decltype(end){} == String{}.end())
    return substr<Begin, End - 1>(String{});
  else
    return substr<Begin, End>(String{});
}

template<typename String>
constexpr auto
strlen(const String&)
{
  return cindex<String{}.size>{};
}

template<typename String, size_t Begin, size_t End>
constexpr auto
strlen(const citerator<String, Begin>&, const citerator<String, End>& end)
{
  if constexpr (decltype(end){} != String{}.end())
    return cindex<End - Begin + 1>{};
  else
    return cindex<End - Begin>{};
}

template<typename Target, typename String>
struct match_size
{
  template<size_t... Indices>
  constexpr size_t match(const ct::index_range<Indices...>&)
  {
    constexpr auto match_char = [](size_t index, auto target, auto string) -> size_t {
      return decltype(target){}[index] == decltype(string){}[index] ? 1 : 0;
    };
    return (0 + ... + match_char(Indices, Target{}, String{}));
  }
};
}
