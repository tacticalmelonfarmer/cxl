#pragma once

#include "utility.hpp"

namespace cxl {
inline namespace literals {

/* template<typename Char, Char... Chars>
constexpr auto operator"" _chr()
{
  static_assert(sizeof...(Chars) == 1, "char literal '_chr' must be a single character");
  return std::integral_constant<char, Chars...>{};
}  //c++20 hopefully*/

template<char... Digits>
constexpr auto operator"" _i()
{
  return std::integral_constant<index_t, combine_digits_base10(0, parse_digit(Digits)...)>{};
}

template<char... Digits>
constexpr auto operator"" _i8()
{
  constexpr auto parsed = combine_digits_base10(0, parse_digit(Digits)...);
  static_assert(parsed >= 0 && parsed <= INT8_MAX, "int8_t literal '_i8' out of bounds");
  return std::integral_constant<int8_t, parsed>{};
}

template<char... Digits>
constexpr auto operator"" _i16()
{
  constexpr auto parsed = combine_digits_base10(0, parse_digit(Digits)...);
  static_assert(parsed >= 0 && parsed <= INT16_MAX, "int16_t literal '_i16' out of bounds");
  return std::integral_constant<int16_t, parsed>{};
}

template<char... Digits>
constexpr auto operator"" _i32()
{
  constexpr auto parsed = combine_digits_base10(0, parse_digit(Digits)...);
  static_assert(parsed >= 0 && parsed <= INT32_MAX, "int32_t literal '_i32' out of bounds");
  return std::integral_constant<int32_t, parsed>{};
}

template<char... Digits>
constexpr auto operator"" _i64()
{
  constexpr auto parsed = combine_digits_base10(0, parse_digit(Digits)...);
  static_assert(parsed >= 0 && parsed <= INT64_MAX, "int64_t literal '_i64' out of bounds");
  return std::integral_constant<int64_t, parsed>{};
}

template<char... Digits>
constexpr auto operator"" _u8()
{
  constexpr auto parsed = combine_digits_base10(0, parse_digit(Digits)...);
  static_assert(parsed >= 0 && parsed <= UINT8_MAX, "uint8_t literal '_u8' out of bounds");
  return std::integral_constant<uint8_t, parsed>{};
}

template<char... Digits>
constexpr auto operator"" _u16()
{
  constexpr auto parsed = combine_digits_base10(0, parse_digit(Digits)...);
  static_assert(parsed >= 0 && parsed <= UINT16_MAX, "uint16_t literal '_u16' out of bounds");
  return std::integral_constant<uint16_t, parsed>{};
}

template<char... Digits>
constexpr auto operator"" _u32()
{
  constexpr auto parsed = combine_digits_base10(0, parse_digit(Digits)...);
  static_assert(parsed >= 0 && parsed <= UINT32_MAX, "uint32_t literal '_u32' out of bounds");
  return std::integral_constant<uint32_t, parsed>{};
}

template<char... Digits>
constexpr auto operator"" _u64()
{
  constexpr auto parsed = combine_digits_base10(0, parse_digit(Digits)...);
  static_assert(parsed >= 0 && parsed <= UINT64_MAX, "uint64_t literal '_u64' out of bounds");
  return std::integral_constant<uint64_t, parsed>{};
}
}
}