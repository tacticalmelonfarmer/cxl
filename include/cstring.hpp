#pragma once
#include "citerator.hpp"
#include "typelist.hpp"
#include "utility.hpp"
#include <type_traits>

namespace utility {

template<char... Chars>
struct cstring
{
  typedef char value_type;

  const std::integral_constant<size_t, sizeof...(Chars)> size = {};
  const char data[sizeof...(Chars) + 1] = { Chars..., '\0' };

  constexpr operator const char*() const { return &data[0]; }
  constexpr char operator[](size_t index) const { return data[index]; }
  constexpr char front() const { return data[0]; }
  constexpr char back() const { return data[sizeof...(Chars) - 1]; }

  constexpr auto begin() const { return citerator<cstring<Chars...>, 0>{}; }
  constexpr auto end() const { return citerator<cstring<Chars...>, sizeof...(Chars)>{}; }
};

#if (defined(__GNUC__) && (__GNUC__ > 5) || (__GNUC__ == 5 && __GNUC_MINOR__ >= 3)) ||                                 \
  (defined(__clang__) && __clang_major__ >= 5)

namespace literals {
template<class Char, Char... Digits>
constexpr auto operator"" _str()
{
  return cstring<Digits...>{};
}
}

#define CSTRING(string_literal) string_literal##_str

#else

namespace literals_alternative {

template<typename String, size_t... Indices>
constexpr auto
build_impl(const index_range<Indices...>)
{
  return cstring<String{}.chars[Indices]...>{};
}

template<size_t Size, typename String>
constexpr auto
build()
{
  return build_impl(make_index_range_t<0, Size - 1>{});
}
}

#define CSTRING(string_literal)                                                                                        \
  [] constexpr                                                                                                         \
  {                                                                                                                    \
    struct constexpr_string_type                                                                                       \
    {                                                                                                                  \
      const char* chars = string_literal;                                                                              \
    };                                                                                                                 \
    return literals_alternative::build<sizeof(string_literal) - 1, constexpr_string_type>();                           \
  }                                                                                                                    \
  ()

#endif

template<char... L, char... R>
constexpr cstring<L..., R...>
operator+(const cstring<L...>, const cstring<R...>)
{
  return cstring<L..., R...>{};
}

template<typename String, size_t... Indices>
constexpr auto
csubstr_impl(const String, const index_range<Indices...>)
{
  return cstring<String{}[Indices]...>{};
}

template<typename String, size_t Begin, size_t End>
constexpr auto
csubstr(const citerator<String, Begin>, const citerator<String, End>)
{
  return csubstr_impl(String{}, make_index_range<Begin, End - 1>());
}

template<typename String>
constexpr auto
csubstr(const citerator<String, 0>, const citerator<String, 0>)
{
  return cstring<>{};
}

template<typename String>
constexpr auto
cstrlen(const String)
{
  return std::integral_constant<size_t, String{}.size>{};
}

template<typename String, size_t Begin, size_t End>
constexpr auto
cstrlen(const citerator<String, Begin>, const citerator<String, End> end)
{
  if constexpr (decltype(end){} != String{}.end())
    return std::integral_constant<size_t, End - Begin + 1>{};
  else
    return std::integral_constant<size_t, End - Begin>{};
}

template<typename Target, typename String>
struct match_size
{
  template<size_t... Indices>
  constexpr size_t match(const index_range<Indices...>)
  {
    constexpr auto match_char = [](size_t index, auto target, auto string) -> size_t {
      return decltype(target){}[index] == decltype(string){}[index] ? 1 : 0;
    };
    return (0 + ... + match_char(Indices, Target{}, String{}));
  }
};

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
}
