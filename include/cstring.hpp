#pragma once
#include "cintegral.hpp"
#include "citerator.hpp"
#include "typelist.hpp"
#include "utility.hpp"
#include <type_traits>

namespace utility {

template<char... Chars>
struct cstring
{
  typedef char value_type;

  constexpr operator const char*() const { return &m_data[0]; }

  template<index_t Index>
  constexpr auto operator[](const std::integral_constant<index_t, Index>) const
  {
    return std::integral_constant<char, m_data[Index]>{};
  }

  constexpr auto size() const { return m_size; }

  constexpr char front() const { return *begin(); }
  constexpr char back() const { return *(--end()); }

  constexpr auto begin() const { return citerator<cstring<Chars...>, 0>{}; }
  constexpr auto end() const { return citerator<cstring<Chars...>, sizeof...(Chars)>{}; }

  constexpr bool operator==(cstring<Chars...>) const { return true; }

  template<char... OtherChars>
  constexpr bool operator==(cstring<OtherChars...>) const
  {
    return false;
  }

private:
  const std::integral_constant<index_t, sizeof...(Chars)> m_size = {};
  const char m_data[sizeof...(Chars) + 1] = { Chars..., '\0' };
};

template<typename String, index_t... Indices>
constexpr auto
build_cstring_impl(const String, const index_range<Indices...>)
{
  return cstring<String{}.chars[Indices]...>{};
}

template<index_t Size, typename String>
constexpr auto
build_cstring()
{
  if constexpr (Size == 0)
    return cstring<>{};
  else
    return build_cstring_impl(String{}, make_index_range<0, Size - 1>());
}

#define STR(string_literal)                                                                                            \
  [] {                                                                                                                 \
    struct constexpr_string_type                                                                                       \
    {                                                                                                                  \
      const char* chars = string_literal;                                                                              \
    };                                                                                                                 \
    return utility::build_cstring<sizeof(string_literal) - 1, constexpr_string_type>();                                \
  }()

#define CHR(char_literal)                                                                                              \
  std::integral_constant<char, char_literal> {}

template<char... L, char... R>
constexpr cstring<L..., R...>
operator+(const cstring<L...>, const cstring<R...>)
{
  return cstring<L..., R...>{};
}

template<typename String, index_t... Indices>
constexpr auto
csubstr_impl(const String, const index_range<Indices...>)
{
  return cstring<String{}[Indices]...>{};
}

template<typename String, index_t Begin, index_t End>
constexpr auto
csubstr(const citerator<String, Begin>, const citerator<String, End>)
{
  return csubstr_impl(String{}, make_index_range<Begin, End - 1>());
}

template<typename String, index_t Pos>
constexpr auto
csubstr(const citerator<String, Pos>, const citerator<String, Pos>)
{
  return cstring<>{};
}

template<typename String>
constexpr auto
cstrlen(const String)
{
  return std::integral_constant<index_t, String{}.size>{};
}

template<typename String, index_t Begin, index_t End>
constexpr auto
cstrlen(const citerator<String, Begin>, const citerator<String, End> end)
{
  if constexpr (decltype(end){} != String{}.end())
    return std::integral_constant<index_t, End - Begin + 1>{};
  else
    return std::integral_constant<index_t, End - Begin>{};
}

template<typename Target, typename String, index_t... Indices>
constexpr auto
cmatch_impl(const Target, const String, const index_range<Indices...>)
{
  constexpr auto match_char = [](index_t index, auto target, auto string) -> index_t {
    return decltype(target){}[index] == decltype(string){}[index] ? 1 : 0;
  };
  return std::integral_constant<index_t, (0 + ... + match_char(Indices, Target{}, String{}))>{};
}

template<typename Target, typename String>
constexpr auto
cmatch(const Target, const String)
{
  return cmatch_impl(Target{}, String{}, make_index_range<0, Target{}.size - 1>());
}

template<typename Target, typename String, typename Begin>
constexpr auto
cfind(const Target, const String, const Begin)
{
  constexpr Target target;
  constexpr String string;
  constexpr Begin begin;
  if constexpr (*begin == target)
    return begin;
  else
    return cfind(target, string, ++begin);
}

template<char Begin, char... Chars>
constexpr auto
cstoi(const cstring<Begin, Chars...>)
{
  if constexpr (Begin == '-') {
    return -(std::integral_constant<int, combine_digits_base10(0, parse_digit(Chars)...)>{});
  } else if constexpr (Begin == '+') {
    return std::integral_constant<int, combine_digits_base10(0, parse_digit(Chars)...)>{};
  } else {
    return std::integral_constant<int, combine_digits_base10(0, parse_digit(Begin), parse_digit(Chars)...)>{};
  }
}

template<char Begin, char... Chars>
constexpr auto
cstol(const cstring<Begin, Chars...>)
{
  if constexpr (Begin == '-') {
    return -(std::integral_constant<long, combine_digits_base10(0, parse_digit(Chars)...)>{});
  } else if constexpr (Begin == '+') {
    return std::integral_constant<long, combine_digits_base10(0, parse_digit(Chars)...)>{};
  } else {
    return std::integral_constant<long, combine_digits_base10(0, parse_digit(Begin), parse_digit(Chars)...)>{};
  }
}

template<char Begin, char... Chars>
constexpr auto
cstoll(const cstring<Begin, Chars...>)
{
  if constexpr (Begin == '-') {
    return -(std::integral_constant<long long, combine_digits_base10(0, parse_digit(Chars)...)>{});
  } else if constexpr (Begin == '+') {
    return std::integral_constant<long long, combine_digits_base10(0, parse_digit(Chars)...)>{};
  } else {
    return std::integral_constant<long long, combine_digits_base10(0, parse_digit(Begin), parse_digit(Chars)...)>{};
  }
}

template<char Begin, char... Chars>
constexpr auto
cstoui(const cstring<Begin, Chars...>)
{
  if constexpr (Begin == '-') {
    return -(std::integral_constant<unsigned int, combine_digits_base10(0, parse_digit(Chars)...)>{});
  } else if constexpr (Begin == '+') {
    return std::integral_constant<unsigned int, combine_digits_base10(0, parse_digit(Chars)...)>{};
  } else {
    return std::integral_constant<unsigned int, combine_digits_base10(0, parse_digit(Begin), parse_digit(Chars)...)>{};
  }
}

template<char Begin, char... Chars>
constexpr auto
cstoul(const cstring<Begin, Chars...>)
{
  if constexpr (Begin == '-') {
    return -(std::integral_constant<unsigned long, combine_digits_base10(0, parse_digit(Chars)...)>{});
  } else if constexpr (Begin == '+') {
    return std::integral_constant<unsigned long, combine_digits_base10(0, parse_digit(Chars)...)>{};
  } else {
    return std::integral_constant<unsigned long, combine_digits_base10(0, parse_digit(Begin), parse_digit(Chars)...)>{};
  }
}

template<char Begin, char... Chars>
constexpr auto
cstoull(const cstring<Begin, Chars...>)
{
  if constexpr (Begin == '-') {
    return -(std::integral_constant<unsigned long long, combine_digits_base10(0, parse_digit(Chars)...)>{});
  } else if constexpr (Begin == '+') {
    return std::integral_constant<unsigned long long, combine_digits_base10(0, parse_digit(Chars)...)>{};
  } else {
    return std::integral_constant<unsigned long long,
                                  combine_digits_base10(0, parse_digit(Begin), parse_digit(Chars)...)>{};
  }
}

template<char Begin, char... Chars>
constexpr float
cstof(const cstring<Begin, Chars...>)
{
  if constexpr (Begin == '-') {
    constexpr auto string = cstring<Chars...>{};
    constexpr auto decimal_iter = cfind(CHR('.'), string, string.begin());
    constexpr float integer_part = cstoi(csubstr(string.begin(), decimal_iter));
    constexpr auto fractional_string = csubstr(++decimal_iter, string.end());
    constexpr float fractional_part = cstoi(fractional_string) * cpow<10, -(fractional_string.size())>();
    return -(integer_part + fractional_part);
  } else if constexpr (Begin == '+') {
    constexpr auto string = cstring<Chars...>{};
    constexpr auto decimal_iter = cfind(CHR('.'), string, string.begin());
    constexpr float integer_part = cstoi(csubstr(string.begin(), decimal_iter));
    constexpr auto fractional_string = csubstr(++decimal_iter, string.end());
    constexpr float fractional_part = cstoi(fractional_string) * cpow<10, -(fractional_string.size())>();
    return integer_part + fractional_part;
  } else {
    constexpr auto string = cstring<Begin, Chars...>{};
    constexpr auto decimal_iter = cfind(CHR('.'), string, string.begin());
    constexpr float integer_part = cstoi(csubstr(string.begin(), decimal_iter));
    constexpr auto fractional_string = csubstr(++decimal_iter, string.end());
    constexpr float fractional_part = cstoi(fractional_string) * cpow<10, -(fractional_string.size())>();
    return integer_part + fractional_part;
  }
}

template<char Begin, char... Chars>
constexpr double
cstod(const cstring<Begin, Chars...>)
{
  if constexpr (Begin == '-') {
    constexpr auto string = cstring<Chars...>{};
    constexpr auto decimal_iter = cfind(CHR('.'), string, string.begin());
    constexpr double integer_part = cstoi(csubstr(string.begin(), decimal_iter));
    constexpr auto fractional_string = csubstr(++decimal_iter, string.end());
    constexpr double fractional_part = cstoi(fractional_string) * cpow<10, -(fractional_string.size)>();
    return -(integer_part + fractional_part);
  } else if constexpr (Begin == '+') {
    constexpr auto string = cstring<Chars...>{};
    constexpr auto decimal_iter = cfind(CHR('.'), string, string.begin());
    constexpr double integer_part = cstoi(csubstr(string.begin(), decimal_iter));
    constexpr auto fractional_string = csubstr(++decimal_iter, string.end());
    constexpr double fractional_part = cstoi(fractional_string) * cpow<10, -(fractional_string.size)>();
    return integer_part + fractional_part;
  } else {
    constexpr auto string = cstring<Begin, Chars...>{};
    constexpr auto decimal_iter = cfind(CHR('.'), string, string.begin());
    constexpr double integer_part = cstoi(csubstr(string.begin(), decimal_iter));
    constexpr auto fractional_string = csubstr(++decimal_iter, string.end());
    constexpr double fractional_part = cstoi(fractional_string) * cpow<10, -(fractional_string.size)>();
    return integer_part + fractional_part;
  }
}
}
