#pragma once
#include "integral.hpp"
#include "iterator.hpp"
#include "typelist.hpp"
#include "utility.hpp"
#include <type_traits>

namespace cxl {

template<char... Chars>
struct string
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

  constexpr auto begin() const { return iterator<string<Chars...>, 0>{}; }
  constexpr auto end() const { return iterator<string<Chars...>, sizeof...(Chars)>{}; }

  constexpr bool operator==(string<Chars...>) const { return true; }

  template<char... OtherChars>
  constexpr bool operator==(string<OtherChars...>) const
  {
    return false;
  }

private:
  const std::integral_constant<index_t, sizeof...(Chars)> m_size = {};
  const char m_data[sizeof...(Chars) + 1] = { Chars..., '\0' };
};

template<typename String, index_t... Indices>
constexpr auto
build_string_impl(const String, const index_range<Indices...>)
{
  return string<String{}.chars[Indices]...>{};
}

template<index_t Size, typename String>
constexpr auto
build_string()
{
  if constexpr (Size == 0)
    return string<>{};
  else
    return build_string_impl(String{}, make_index_range<0, Size - 1>());
}

#define STR(string_literal)                                                                                            \
  []() constexpr                                                                                                       \
  {                                                                                                                    \
    struct constexpr_string_type                                                                                       \
    {                                                                                                                  \
      const char* chars = string_literal;                                                                              \
    };                                                                                                                 \
    return cxl::build_string<sizeof(string_literal) - 1, constexpr_string_type>();                                     \
  }                                                                                                                    \
  ()

#define CHR(char_literal)                                                                                              \
  std::integral_constant<char, char_literal> {}

template<char... L, char... R>
constexpr string<L..., R...>
operator+(const string<L...>, const string<R...>)
{
  return string<L..., R...>{};
}

template<typename String, index_t... Indices>
constexpr auto
substr_impl(const String, const index_range<Indices...>)
{
  return string<String{}[Indices]...>{};
}

template<typename String, index_t Begin, index_t End>
constexpr auto
substr(const iterator<String, Begin>, const iterator<String, End>)
{
  return substr_impl(String{}, make_index_range<Begin, End - 1>());
}

template<typename String, index_t Pos>
constexpr auto
substr(const iterator<String, Pos>, const iterator<String, Pos>)
{
  return string<>{};
}

template<typename String>
constexpr auto
strlen(const String)
{
  return std::integral_constant<index_t, String{}.size>{};
}

template<typename String, index_t Begin, index_t End>
constexpr auto
strlen(const iterator<String, Begin>, const iterator<String, End> end)
{
  if constexpr (decltype(end){} != String{}.end())
    return std::integral_constant<index_t, End - Begin + 1>{};
  else
    return std::integral_constant<index_t, End - Begin>{};
}

template<typename Target, typename String, index_t... Indices>
constexpr auto
strmatch_impl(const Target, const String, const index_range<Indices...>)
{
  constexpr auto match_char = [](index_t index) -> index_t { return Target{}[index] == String{}[index] ? 1 : 0; };
  return std::integral_constant<index_t, (0 + ... + match_char(Indices))>{};
}

template<typename Target, typename String>
constexpr auto
strmatch(const Target, const String)
{
  return strmatch_impl(Target{}, String{}, make_index_range<0, Target{}.size() - 1>());
}

template<typename Target, typename String, typename Begin>
constexpr auto
find(const Target, const String, const Begin)
{
  constexpr Target target;
  constexpr String string;
  constexpr Begin begin;
  if constexpr (*begin == target)
    return begin;
  else
    return find(target, string, ++begin);
}

template<char Begin, char... Chars>
constexpr auto
stoi(const string<Begin, Chars...>)
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
stol(const string<Begin, Chars...>)
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
stoll(const string<Begin, Chars...>)
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
stoui(const string<Begin, Chars...>)
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
stoul(const string<Begin, Chars...>)
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
stoull(const string<Begin, Chars...>)
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
stof(const string<Begin, Chars...>)
{
  if constexpr (Begin == '-') {
    constexpr auto work_string = string<Chars...>{};
    constexpr auto decimal_iter = find(CHR('.'), work_string, work_string.begin());
    constexpr float integer_part = stoi(csubstr(work_string.begin(), decimal_iter));
    constexpr auto fractional_string = substr(++decimal_iter, work_string.end());
    constexpr float fractional_part = stoi(fractional_string) * pow<10, -(fractional_string.size())>();
    return -(integer_part + fractional_part);
  } else if constexpr (Begin == '+') {
    constexpr auto work_string = string<Chars...>{};
    constexpr auto decimal_iter = find(CHR('.'), work_string, work_string.begin());
    constexpr float integer_part = stoi(csubstr(work_string.begin(), decimal_iter));
    constexpr auto fractional_string = substr(++decimal_iter, work_string.end());
    constexpr float fractional_part = stoi(fractional_string) * pow<10, -(fractional_string.size())>();
    return integer_part + fractional_part;
  } else {
    constexpr auto work_string = string<Begin, Chars...>{};
    constexpr auto decimal_iter = find(CHR('.'), work_string, work_string.begin());
    constexpr float integer_part = stoi(csubstr(work_string.begin(), decimal_iter));
    constexpr auto fractional_string = substr(++decimal_iter, work_string.end());
    constexpr float fractional_part = stoi(fractional_string) * pow<10, -(fractional_string.size())>();
    return integer_part + fractional_part;
  }
}

template<char Begin, char... Chars>
constexpr double
stod(const string<Begin, Chars...>)
{
  if constexpr (Begin == '-') {
    constexpr auto work_string = string<Chars...>{};
    constexpr auto decimal_iter = find(CHR('.'), work_string, work_string.begin());
    constexpr double integer_part = stoi(csubstr(work_string.begin(), decimal_iter));
    constexpr auto fractional_string = substr(++decimal_iter, work_string.end());
    constexpr double fractional_part = stoi(fractional_string) * pow<10, -(fractional_string.size)>();
    return -(integer_part + fractional_part);
  } else if constexpr (Begin == '+') {
    constexpr auto work_string = string<Chars...>{};
    constexpr auto decimal_iter = find(CHR('.'), work_string, work_string.begin());
    constexpr double integer_part = stoi(csubstr(work_string.begin(), decimal_iter));
    constexpr auto fractional_string = substr(++decimal_iter, work_string.end());
    constexpr double fractional_part = stoi(fractional_string) * pow<10, -(fractional_string.size)>();
    return integer_part + fractional_part;
  } else {
    constexpr auto work_string = string<Begin, Chars...>{};
    constexpr auto decimal_iter = find(CHR('.'), work_string, work_string.begin());
    constexpr double integer_part = stoi(csubstr(work_string.begin(), decimal_iter));
    constexpr auto fractional_string = substr(++decimal_iter, work_string.end());
    constexpr double fractional_part = stoi(fractional_string) * pow<10, -(fractional_string.size)>();
    return integer_part + fractional_part;
  }
}
}
